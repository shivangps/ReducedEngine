#include "Cubemap.h"

#include "../Assets/CubemapShader/CubemapShader.h"

void Cubemap::Initialize(Microsoft::WRL::ComPtr<ID3D12Device5> device, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList, std::string skymapFileDirectory)
{
	HRESULT HR;

	// SHADER RESOURCE VIEW
	std::unique_ptr<uint8_t[]> decodedData;
	std::vector<D3D12_SUBRESOURCE_DATA> subresources;
	DirectX::DDS_ALPHA_MODE alphaMode = DirectX::DDS_ALPHA_MODE_UNKNOWN;
	bool isCubemap = true;

	
	HR = LoadDDSTextureFromFile(device.Get(), StringToWide(skymapFileDirectory).c_str(), &this->textureResource, decodedData, subresources, 0Ui64, &alphaMode, &isCubemap);
	if (FAILED(HR))
	{
		ErrorLog(HR, "Failed to load the dds cubemap.");
		exit(-1);
	}

	const UINT64 uploadBufferSize = GetRequiredIntermediateSize(this->textureResource.Get(), 0, subresources.size());

	HR = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(this->uploadResource.GetAddressOf())
	);
	if (FAILED(HR))
	{
		ErrorLog(HR, "Failed to create upload resource for skybox");
		exit(-1);
	}

	UpdateSubresources(commandList.Get(), this->textureResource.Get(), this->uploadResource.Get(), 0, 0, subresources.size(), &subresources[0]);
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(this->textureResource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.TextureCube.MostDetailedMip = 0;
	srvDesc.TextureCube.MipLevels = this->textureResource->GetDesc().MipLevels;
	srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
	srvDesc.Format = this->textureResource->GetDesc().Format;

	this->textureHandle = this->universalDescriptorHeap->GetCbvSrvUavGPUHandle(this->universalDescriptorHeap->SetCpuHandle(device, this->textureResource.Get(), &srvDesc));

	// CONSTANT BUFFER VIEW
	HR = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(matrix) + 255) & ~255),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(this->constantResource.GetAddressOf()));

	if (FAILED(HR))
	{
		ErrorLog(HR, "Failed to create buffer resource for cubemap transform matrix.");
		exit(-1);
	}

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = this->constantResource->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = (sizeof(matrix) + 255) & ~255;

	this->constantBufferHandle = this->universalDescriptorHeap->GetCbvSrvUavGPUHandle(this->universalDescriptorHeap->SetCpuHandle(device, &cbvDesc));

	CD3DX12_RANGE readRange(0, 0);
	HR = this->constantResource->Map(0, &readRange, reinterpret_cast<void**>(&this->pCBVMatrix));
	if (FAILED(HR))
	{
		ErrorLog(HR, "Failed to map the generic CBV.");
		exit(-1);
	}
	this->constantResource->Unmap(0, nullptr);

	// SHADER INITIALIZATION
	shader = CubemapShader::GetInstance();

	// MESH INITIALIZATION
	SkyboxVertex vertices[] =
	{
			SkyboxVertex(-1.0f,  1.0f, -1.0f),
			SkyboxVertex(-1.0f, -1.0f, -1.0f),
			SkyboxVertex(1.0f, -1.0f, -1.0f),
			SkyboxVertex(1.0f, -1.0f, -1.0f),
			SkyboxVertex(1.0f,  1.0f, -1.0f),
			SkyboxVertex(-1.0f,  1.0f, -1.0f),

			SkyboxVertex(-1.0f, -1.0f,  1.0f),
			SkyboxVertex(-1.0f, -1.0f, -1.0f),
			SkyboxVertex(-1.0f,  1.0f, -1.0f),
			SkyboxVertex(-1.0f,  1.0f, -1.0f),
			SkyboxVertex(-1.0f,  1.0f,  1.0f),
			SkyboxVertex(-1.0f, -1.0f,  1.0f),

			SkyboxVertex(1.0f, -1.0f, -1.0f),
			SkyboxVertex(1.0f, -1.0f,  1.0f),
			SkyboxVertex(1.0f,  1.0f,  1.0f),
			SkyboxVertex(1.0f,  1.0f,  1.0f),
			SkyboxVertex(1.0f,  1.0f, -1.0f),
			SkyboxVertex(1.0f, -1.0f, -1.0f),

			SkyboxVertex(-1.0f, -1.0f,  1.0f),
			SkyboxVertex(-1.0f,  1.0f,  1.0f),
			SkyboxVertex(1.0f,  1.0f,  1.0f),
			SkyboxVertex(1.0f,  1.0f,  1.0f),
			SkyboxVertex(1.0f, -1.0f,  1.0f),
			SkyboxVertex(-1.0f, -1.0f,  1.0f),

			SkyboxVertex(-1.0f,  1.0f, -1.0f),
			SkyboxVertex(1.0f,  1.0f, -1.0f),
			SkyboxVertex(1.0f,  1.0f,  1.0f),
			SkyboxVertex(1.0f,  1.0f,  1.0f),
			SkyboxVertex(-1.0f,  1.0f,  1.0f),
			SkyboxVertex(-1.0f,  1.0f, -1.0f),

			SkyboxVertex(-1.0f, -1.0f, -1.0f),
			SkyboxVertex(-1.0f, -1.0f,  1.0f),
			SkyboxVertex(1.0f, -1.0f, -1.0f),
			SkyboxVertex(1.0f, -1.0f, -1.0f),
			SkyboxVertex(-1.0f, -1.0f,  1.0f),
			SkyboxVertex(1.0f, -1.0f,  1.0f)

	};

	NumVertices = ARRAYSIZE(vertices);
	unsigned int verticesSize = sizeof(vertices);

	HRESULT hr = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(verticesSize),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(VertexUploadBuffer.GetAddressOf()));
	if (FAILED(hr))
	{
		ErrorLog(hr, "Failed to create upload buffer for vertices.");
		exit(-1);
	}
	VertexUploadBuffer->SetName(L"Skybox Vertex Upload Buffer");

	UINT8* pMeshVertexDataBegin;
	hr = VertexUploadBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pMeshVertexDataBegin));
	if (FAILED(hr))
	{
		ErrorLog(hr, "Failed to map polygon vertex resource.");
		exit(-1);
	}
	memcpy(pMeshVertexDataBegin, &vertices[0], verticesSize);
	VertexUploadBuffer->Unmap(0, nullptr);

	VertexBufferView.BufferLocation = VertexUploadBuffer->GetGPUVirtualAddress();
	VertexBufferView.StrideInBytes = sizeof(SkyboxVertex);
	VertexBufferView.SizeInBytes = verticesSize;
}

void Cubemap::Draw(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList, Camera camera)
{
	Vector3 position = camera.GetTransform().GetGlobalPostion();

	Matrix4 model;
	model = model.Translation(position);

	this->matrix.viewProj = camera.GetViewProjectionMatrix().Transpose();
	this->matrix.model = model.Transpose();
	this->matrix.projection = camera.projection.Transpose();
	this->matrix.view = camera.view.Transpose();
	memcpy(pCBVMatrix, &this->matrix, sizeof(this->matrix));

	shader->SetShaderForRender(commandList);

	commandList->SetGraphicsRootDescriptorTable(0, this->textureHandle);

	commandList->SetGraphicsRootDescriptorTable(1, this->constantBufferHandle);

	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->IASetVertexBuffers(0, 1, &VertexBufferView);
	commandList->DrawInstanced(this->NumVertices, 1, 0, 0);
}
