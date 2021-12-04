#include "RenderComponent.h"

void RenderComponent::InitializeLocalData(Microsoft::WRL::ComPtr<ID3D12Device5> device)
{
	// Create Resource.
	unsigned int size = sizeof(this->localData);
	InitializeResource(&this->localDataResource, device, &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), &CD3DX12_RESOURCE_DESC::Buffer(GetAggregateSize(size)), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr);
	this->localDataResource->SetName(L"Render Component Local Data Resource");

	// Create constant buffer view.
	D3D12_CONSTANT_BUFFER_VIEW_DESC constantBufferViewDesc = {};
	constantBufferViewDesc.BufferLocation = this->localDataResource->GetGPUVirtualAddress();
	constantBufferViewDesc.SizeInBytes = GetAggregateSize(size);
	device->CreateConstantBufferView(&constantBufferViewDesc, this->heap.GetCPUHandle(0));

	// Create data pointer to cbv memory space.
	CD3DX12_RANGE readRange = {};
	this->localDataResource->Map(0, &readRange, reinterpret_cast<void**>(&this->pLocalDataCBV));
	this->localDataResource->Unmap(0, nullptr);
}

void RenderComponent::UpdateCameraMatrix(Camera camera)
{
	Transform transform = *this->transform;

	Matrix4 model;

	model = model.Rotation(transform.GetGlobalRotation());
	model = model.Scale(transform.GetGlobalScale());
	model = model.Translation(transform.GetGlobalPostion());

	this->localData.cameraMatrix = model * camera.GetViewProjectionMatrix();
	this->localData.modelMatrix = model.Transpose();
	this->localData.viewMatrix = camera.view.Transpose();
	this->localData.normalMatrix = (camera.view * model).Inverse();
	this->localData.modelViewMatrix = (model * camera.view).Transpose();

	memcpy(this->pLocalDataCBV, &this->localData, sizeof(this->localData));
}

void RenderComponent::InitializeNewMesh(std::vector<MeshVertex> vertices, std::vector<unsigned short> indices)
{
	MeshData meshData = {};

	meshData.vertices = vertices;
	meshData.indices = indices;
	meshData.colorTextureFileDirectory = DefaultTextureDirectory::WhiteTexture;

	this->meshDataToBeLoaded.push_back(meshData);
}

void RenderComponent::InitializeNewMesh(std::vector<MeshVertex> vertices, std::vector<unsigned short> indices, std::string colorTextureFileDirectory)
{
	MeshData meshData = {};

	meshData.vertices = vertices;
	meshData.indices = indices;
	meshData.colorTextureFileDirectory = colorTextureFileDirectory;

	this->meshDataToBeLoaded.push_back(meshData);
}

void RenderComponent::LoadModel(std::string fileDirectory)
{
	this->modelFileLocation.push_back(fileDirectory);
}

void RenderComponent::SetShaderToComponent(Shader* shader)
{
	this->shader = shader;
}

void RenderComponent::InitializeComponent(Microsoft::WRL::ComPtr<ID3D12Device5> device, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList)
{
	this->heap.Initialize(device, 2, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
	GameAssetManager* GAM = GameAssetManager::GetInstance();

	// Load custom meshes.
	for (unsigned int i = 0; i < this->meshDataToBeLoaded.size(); i++)
	{
		MeshData meshData = {};
		meshData = this->meshDataToBeLoaded[i];
		Mesh* mesh = GAM->GetMesh(GAM->SetNewMesh(meshData.vertices, meshData.indices));
		this->meshes.push_back({ mesh, mesh->GetVertexBufferView(), mesh->GetIndexBufferView(), mesh->GetNumberOfVertices(), mesh->GetNumberOfIndices() });

		this->colorTextureIndex = GAM->SetNewTexture(meshData.colorTextureFileDirectory);
		GAM->AssignTextureViewToHeap(this->colorTextureIndex, this->heap.GetCPUHandle(1));
	}
	this->meshDataToBeLoaded.clear();

	// Load models.
	for (unsigned int i = 0; i < this->modelFileLocation.size(); i++)
	{
		ModelInfo modelInfo = {};
		modelInfo = GAM->LoadModel(this->modelFileLocation[i]);

		for (unsigned int j = 0; j < modelInfo.modelFragments.size(); j++)
		{
			Mesh* mesh = GAM->GetMesh(modelInfo.modelFragments[j].meshIndex);
			this->meshes.push_back({ mesh, mesh->GetVertexBufferView(), mesh->GetIndexBufferView(), mesh->GetNumberOfVertices(), mesh->GetNumberOfIndices() });

			this->colorTextureIndex = modelInfo.modelFragments[j].colorTextureIndex;
			GAM->AssignTextureViewToHeap(this->colorTextureIndex, this->heap.GetCPUHandle(1));
		}
	}

	// Initialize local data such as camera matrix.
	this->InitializeLocalData(device);

	// Initialize shadow descriptor heap.
	this->shadowDataHeap.Initialize(device, 1, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);

	// Initialze the shadow data constant buffer.
	this->InitializeShadowConstantBuffer(device);
}

void RenderComponent::Draw(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList, Camera camera)
{
	this->UpdateCameraMatrix(camera);

	this->shader->SetShaderForRender(commandList);

	ID3D12DescriptorHeap* ppHeaps[] = { this->heap.Get() };
	commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	commandList->SetGraphicsRootDescriptorTable(0, this->heap.GetGPUHandle(0));
	commandList->SetGraphicsRootDescriptorTable(1, this->heap.GetGPUHandle(1));

	for (unsigned int i = 0; i < this->meshes.size(); i++)
	{
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		commandList->IASetVertexBuffers(0, 1, &this->meshes[i].vertexBufferView);
		commandList->IASetIndexBuffer(&this->meshes[i].indexBufferView);
		commandList->DrawIndexedInstanced(this->meshes[i].numberOfIndices, 1, 0, 0, 0);
	}
}

void RenderComponent::InitializeShadowConstantBuffer(Microsoft::WRL::ComPtr<ID3D12Device5> device)
{
	// Create resource. 
	unsigned int size = sizeof(this->shadowData);
	InitializeResource(&this->shadowDataResource, device, &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), &CD3DX12_RESOURCE_DESC::Buffer(GetAggregateSize(size)), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr);
	this->shadowDataResource->SetName(L"Render Component Shadow Data Resource");

	// Create constant buffer view.
	D3D12_CONSTANT_BUFFER_VIEW_DESC constantBufferViewDesc = {};
	constantBufferViewDesc.BufferLocation = this->shadowDataResource->GetGPUVirtualAddress();
	constantBufferViewDesc.SizeInBytes = GetAggregateSize(size);
	device->CreateConstantBufferView(&constantBufferViewDesc, this->shadowDataHeap.GetCPUHandle(0));

	// Create data pointer to cbv memory space.
	CD3DX12_RANGE readRange = {};
	this->shadowDataResource->Map(0, &readRange, reinterpret_cast<void**>(&this->pShadowDataCBV));
	this->shadowDataResource->Unmap(0, nullptr);
}

void RenderComponent::DrawForShadow(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList, Matrix4 lightSpaceMatrix)
{
	// Update objects transform for constant buffer update.
	Transform transform = *this->transform;

	Matrix4 model;

	model = model.Translation(transform.GetGlobalPostion());
	model = model.Rotation(transform.GetGlobalRotation());
	model = model.Scale(transform.GetGlobalScale());

	this->shadowData.lightSpaceMatrix = model * lightSpaceMatrix;

	memcpy(this->pShadowDataCBV, &this->shadowData, sizeof(this->shadowData));

	ID3D12DescriptorHeap* ppHeaps[] = { this->shadowDataHeap.Get() };
	commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	commandList->SetGraphicsRootDescriptorTable(0, this->shadowDataHeap.GetGPUHandle(0));

	for (unsigned int i = 0; i < this->meshes.size(); i++)
	{
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		commandList->IASetVertexBuffers(0, 1, &this->meshes[i].vertexBufferView);
		commandList->IASetIndexBuffer(&this->meshes[i].indexBufferView);
		commandList->DrawIndexedInstanced(this->meshes[i].numberOfIndices, 1, 0, 0, 0);
	}
}

void RenderComponent::SetEnable(Boolean* pBool)
{
	this->enable = pBool;
}

void RenderComponent::EnableRendering()
{
	this->enable->SetTrue();
}

void RenderComponent::DisableRendering()
{
	this->enable->SetFalse();
}
