#pragma once
#include <vector>
#include "Texture.h"
#include "../Output/Graphics/GraphicsHelper.h"
#include "../Output/Graphics/UniversalDescriptorHeap.h"

// This header file consists a singleton(single instance) class that maintains the texture assets loaded on GPU RAM.
namespace DefaultTextureDirectory
{
	static const std::string WhiteTexture = "Assets/Textures/White.jpg";
	static const std::string BlackTexture = "Assets/Textures/Black.jpg";
	static const std::string NormalTexture = "Assets/Textures/Normal.jpg";
}

class TextureAssetManager
{

	struct TextureAssetInfo
	{
		UINT64 textureIndex = 0;
		Texture texture;
		D3D12_GPU_DESCRIPTOR_HANDLE handle = {};
	};

	UniversalDescriptorHeap* universalDescriptorHeap = UniversalDescriptorHeap::GetInstance();

	Microsoft::WRL::ComPtr<ID3D12Device5> graphicsDevice = nullptr;

	std::vector<TextureAssetInfo*> textures = {};
	UINT64 currentTextureIndex = 0;

	TextureAssetManager() {}

public:
	// Get single instance of the class.
	static TextureAssetManager* GetInstance()
	{
		static TextureAssetManager* instance = new TextureAssetManager();
		return instance;
	}

	// Function to set the device for loading all the texture data to device(GPU) RAM.(To be called only by Graphics Engine)
	void SetDevice(Microsoft::WRL::ComPtr<ID3D12Device5> device);
	// Function to call for loading all the texture data to CPU RAM.
	void LoadAllTextureDataTo_GPU_RAM(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList);
	// Function to call for removal of all texture data from CPU RAM.
	void RemoveAllTextureDataFrom_CPU_RAM();


	// Function to get the texture pointer base on texture index entered.
	Texture* GetTexture(UINT64 textureIndex);
	// Function to create resource view from texture and store it in descriptor heap.
	void CreateResourceView(UINT64 textureIndex, D3D12_CPU_DESCRIPTOR_HANDLE handle);
	// Function to assign a new texture and assigns as well as returns the index of the texture.
	UINT64 SetNewTexture(std::string fileDirectory);
	// Function to get the GPU descriptor handle for texture.
	D3D12_GPU_DESCRIPTOR_HANDLE GetTextureHandleForRender(UINT64 textureIndex);
	// Function to release all the texture resources in the asset manager.
	void ReleaseAllTextures();
};