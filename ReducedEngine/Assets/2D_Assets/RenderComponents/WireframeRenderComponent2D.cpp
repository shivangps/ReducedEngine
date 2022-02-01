#include "WireframeRenderComponent2D.h"
#include "../Assets/2D_Assets/Shaders/WireframeShader/WireframeShader.h"

void WireframeRenderComponent2D::InitializePixelDataConstantBuffer(Microsoft::WRL::ComPtr<ID3D12Device5> device, LPCWSTR constantBufferName)
{
	this->pixelDataConstantBuffer.Initialize(device, sizeof(PixelData), constantBufferName);
	UniversalDescriptorHeap* UDH = UniversalDescriptorHeap::GetInstance();
	this->pixelDataConstantBufferHandle = UDH->GetCbvSrvUavGPUHandle(UDH->SetCpuHandle(device, this->pixelDataConstantBuffer.GetConstantBufferViewDesc()));

	this->pixelDataConstantBuffer.CopyDataToConstantBufferLocation(&this->pixelData);
}

void WireframeRenderComponent2D::Initialize(Microsoft::WRL::ComPtr<ID3D12Device5> device)
{
	this->InitializeMatrixConstantBuffer(device, L"Wireframe Matrix");

	this->wireframeShader = WireframeShader::GetInstance();

	GameAssetManager* gameAssetManager = GameAssetManager::GetInstance();

	// Load custom geometry.
	for (unsigned int i = 0; i < this->geometryDataToBeLoaded.size(); i++)
	{
		Geometry2DData geometryData = {};
		geometryData = this->geometryDataToBeLoaded[i];
		Mesh* geometry = gameAssetManager->GetMesh(gameAssetManager->SetNewGeometry2D(geometryData.vertices, geometryData.indices));
		this->geometries.push_back({geometry, geometry->GetVertexBufferView(), geometry->GetIndexBufferView(), geometry->GetNumberOfVertices(), geometry->GetNumberOfIndices()});

		this->albedoTextureIndex = gameAssetManager->SetNewTexture(geometryData.albedoTextureFileDirectory);

		this->albedoTextureHandle = gameAssetManager->GetTextureHandle(this->albedoTextureIndex);
	}

	this->geometryDataToBeLoaded.clear();

	this->InitializePixelDataConstantBuffer(device, L"Wireframe Pixel Data");
}

void WireframeRenderComponent2D::Draw(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList, Camera2D camera)
{
	this->wireframeShader->SetShaderForRender(commandList);

	this->UpdateMatrixConstantBuffer(camera);

	commandList->SetGraphicsRootDescriptorTable(WireframeShader::CBSR_Slot::matrixDataSlot, this->matrixConstantBufferHandle);
	commandList->SetGraphicsRootDescriptorTable(WireframeShader::CBSR_Slot::localPixelDataSlot, this->pixelDataConstantBufferHandle);

	for (unsigned int i = 0; i < this->geometries.size(); i++)
	{
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		commandList->IASetVertexBuffers(0, 1, &this->geometries[i].vertexBufferView);
		commandList->IASetIndexBuffer(&this->geometries[i].indexBufferView);
		commandList->DrawIndexedInstanced(this->geometries[i].numberOfIndices, 1, 0, 0, 0);
	}
}

void WireframeRenderComponent2D::Load2DGeometry(std::vector<MeshVertex2D> vertices, std::vector<unsigned short> indices)
{
	Geometry2DData geometryData;

	geometryData.vertices = vertices;
	geometryData.indices = indices;
	geometryData.albedoTextureFileDirectory = DefaultTextureDirectory::WhiteTexture;

	this->geometryDataToBeLoaded.push_back(geometryData);
}

void WireframeRenderComponent2D::ChangeColor(Vector3 color)
{
	this->pixelData.color = color;
	if(this->pixelDataConstantBuffer.IsInitialized())
		this->pixelDataConstantBuffer.CopyDataToConstantBufferLocation(&this->pixelData);
}
