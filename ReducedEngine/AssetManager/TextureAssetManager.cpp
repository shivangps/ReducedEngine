#include "TextureAssetManager.h"

#include "../ErrorLogger.h"

void TextureAssetManager::SetDevice(Microsoft::WRL::ComPtr<ID3D12Device5> device)
{
	this->graphicsDevice = device;

	this->texturesHeap.Initialize(this->graphicsDevice, 100, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
}

int TextureAssetManager::SetNewTexture(std::string fileDirectory)
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
		textureInfo->texture.CreateResourceView(this->graphicsDevice, this->texturesHeap.GetCPUHandle(textureInfo->textureIndex));

		return textureInfo->textureIndex;
	}
	else
	{
		ExitWithMessage("Graphics Device not assigned to Texture Asset Manager.");
	}
	return -1;
}

D3D12_GPU_DESCRIPTOR_HANDLE TextureAssetManager::GetTextureHandleForRender(UINT64 textureIndex)
{
	return this->texturesHeap.GetGPUHandle(textureIndex);
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

void TextureAssetManager::SetDescriptorHeap(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList)
{
	ID3D12DescriptorHeap* ppHeaps[] = { this->texturesHeap.Get() };
	commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
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
