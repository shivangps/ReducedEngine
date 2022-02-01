#pragma once
#include "../Output/Graphics/d3dx12.h"

// This header file contains the class for loading the texture file.
// And applying for rendering.

class Texture
{
public:
	Texture() {}

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> mainTextureResource = {};
	Microsoft::WRL::ComPtr<ID3D12Resource> uploadResource = {};
	std::unique_ptr<D3D12_SUBRESOURCE_DATA> subresources = std::unique_ptr<D3D12_SUBRESOURCE_DATA>(new D3D12_SUBRESOURCE_DATA());
	std::unique_ptr<unsigned char[]> decodedData;

public:
	// Function to initialize by loading textures from files. Returns false when file not found.
	void Initialize(Microsoft::WRL::ComPtr<ID3D12Device5> device, std::string fileDirectory);
	// Function to load texture data to GPU RAM.
	void LoadTextureDataTo_GPU_RAM(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList);
	// Function to remove all the texture data from CPU RAM.
	void RemoveTextureDataFrom_CPU_RAM();
	// Function to create shader resource view from this texture.
	void CreateResourceView(Microsoft::WRL::ComPtr<ID3D12Device5> device, D3D12_CPU_DESCRIPTOR_HANDLE handle);
	// Function to get the texture resource.
	ID3D12Resource* GetResource();
	// Function to release the texture resource.
	void Release();
};