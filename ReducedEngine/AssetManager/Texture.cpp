#include "Texture.h"
#include "..//ErrorLogger.h"
#include "../Output/Graphics/GraphicsHelper.h"
#include "..//Helper.h"
#include <DirectXTK/WICTextureLoader.h>

void Texture::Initialize(Microsoft::WRL::ComPtr<ID3D12Device5> device, std::string fileDirectory)
{
	HRESULT HR;

	this->subresources = std::unique_ptr<D3D12_SUBRESOURCE_DATA>(new D3D12_SUBRESOURCE_DATA());
	HR = DirectX::LoadWICTextureFromFile(device.Get(), StringToWide(fileDirectory).c_str(), this->mainTextureResource.GetAddressOf(), this->decodedData, *this->subresources);
	ExitOnError(HR, "Failed to load texture data from file directory: " + fileDirectory);
	this->mainTextureResource->SetName(StringToWide(GetFileNameFromDirectory(fileDirectory) + " Texture Resource").c_str());

	const unsigned long long uploadBufferSize = GetRequiredIntermediateSize(this->mainTextureResource.Get(), 0, 1);

	InitializeResource(&this->uploadResource, device, &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), &CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr);
}

void Texture::LoadTextureDataTo_GPU_RAM(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList)
{
	UpdateSubresources(commandList.Get(), this->mainTextureResource.Get(), this->uploadResource.Get(), 0, 0, 1, this->subresources.get());
	TransitionResourceState(this->mainTextureResource, commandList, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}

void Texture::RemoveTextureDataFrom_CPU_RAM()
{
	this->decodedData.release();
	this->subresources.release();
}

void Texture::CreateResourceView(Microsoft::WRL::ComPtr<ID3D12Device5> device, D3D12_CPU_DESCRIPTOR_HANDLE handle)
{
	device->CreateShaderResourceView(this->mainTextureResource.Get(), nullptr, handle);
}

ID3D12Resource* Texture::GetResource()
{
	return this->mainTextureResource.Get();
}
