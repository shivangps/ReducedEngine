#include "TextureAssetManager.h"

#include "../ErrorLogger.h"

void TextureAssetManager::SetDevice(Microsoft::WRL::ComPtr<ID3D12Device5> device)
{
	this->graphicsDevice = device;
}

UINT64 TextureAssetManager::SetNewTexture(std::string fileDirectory)
{
	if (this->graphicsDevice)
	{
		TextureAssetInfo* textureInfo = new TextureAssetInfo();

		// Assign a new texture index and initialize the texture.
		textureInfo->textureIndex = this->currentTextureIndex++;
		textureInfo->texture.Initialize(this->graphicsDevice, fileDirectory);

		// Push the texture details into the stack.
		this->textures.push_back(textureInfo);

		// Assign the texture view into the descriptor heap.
		textureInfo->handle = this->universalDescriptorHeap->GetCbvSrvUavGPUHandle(this->universalDescriptorHeap->SetCpuHandle(
			this->graphicsDevice, textureInfo->texture.GetResource(), textureInfo->texture.GetShaderResourceView()));

		return textureInfo->textureIndex;
	}
	else
	{
		ExitWithMessage("Graphics Device not assigned to Texture Asset Manager.");
	}
	return 0;
}

D3D12_GPU_DESCRIPTOR_HANDLE TextureAssetManager::GetTextureHandleForRender(UINT64 textureIndex)
{
	D3D12_GPU_DESCRIPTOR_HANDLE handle = this->textures[0]->handle;		// Assigning white texture handle for fail safe.

	if (textureIndex <= this->currentTextureIndex)
		for (unsigned int i = 0; i < this->textures.size(); i++)
		{
			if (textureIndex == this->textures[i]->textureIndex)
				handle = this->textures[i]->handle;
		}

	return handle;
}

void TextureAssetManager::ReleaseAllTextures()
{
	for (unsigned int i = 0; i < this->textures.size(); i++)
	{
		this->textures[i]->texture.Release();
	}

	this->textures.clear();
}

void TextureAssetManager::LoadAllTextureDataTo_GPU_RAM(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList)
{
	for (unsigned int i = 0; i < this->textures.size(); i++)
	{
		this->textures[i]->texture.LoadTextureDataTo_GPU_RAM(commandList);
	}
}

void TextureAssetManager::RemoveAllTextureDataFrom_CPU_RAM()
{
	for (unsigned int i = 0; i < this->textures.size(); i++)
	{
		this->textures[i]->texture.RemoveTextureDataFrom_CPU_RAM();
	}
}

Texture* TextureAssetManager::GetTexture(UINT64 textureIndex)
{
	if(textureIndex <= this->currentTextureIndex)
		for (unsigned int i = 0; i < this->textures.size(); i++)
		{
			if (textureIndex == this->textures[i]->textureIndex)
				return &this->textures[i]->texture;
		}

	return nullptr;
}

void TextureAssetManager::CreateResourceView(UINT64 textureIndex, D3D12_CPU_DESCRIPTOR_HANDLE handle)
{
	this->GetTexture(textureIndex)->CreateResourceView(this->graphicsDevice, handle);
}
