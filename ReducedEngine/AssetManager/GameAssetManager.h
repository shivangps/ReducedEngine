#pragma once
#include "MeshAssetManager.h"
#include "TextureAssetManager.h"

// Headers to import 3D models.
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/material.h>

// This header file consists of a singleton(single instance) class that manages the assets of the game engine.

// Structure that stores the model information like file name mesh data stored in MeshAssetManager and texture and material index in TextureAssetManager.
struct ModelFragment
{
	UINT64 meshIndex;
	UINT64 colorTextureIndex;
};
struct ModelInfo
{
	std::string fileName;
	std::vector<ModelFragment> modelFragments;
};

class GameAssetManager
{
	GameAssetManager() {}

	MeshAssetManager* Mesh_AM = MeshAssetManager::GetInstance();
	TextureAssetManager* Texture_AM = TextureAssetManager::GetInstance();

public:
	static GameAssetManager* GetInstance()
	{
		static GameAssetManager* instance = new GameAssetManager();
		return instance;
	}

	// Function to assign the graphics device to initialze the meshes and textures.
	void SetGraphicsDevice(Microsoft::WRL::ComPtr<ID3D12Device5> device);
	// Function to initialize all the shaders.
	void InitializeAllShadersForDeferredRender(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int numberOfRenderTarget,
		DXGI_FORMAT* renderTargetFormats, DXGI_FORMAT depthStencilFormat, unsigned int samples);
	// Function to load all the mesh and textures on GPU RAM.
	void LoadAllAssetsTo_GPU_RAM(Microsoft::WRL::ComPtr<ID3D12Device5> device, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList);
	// Function to remove all assets from CPU RAM (WARNING: To be called after loading assets to GPU RAM).
	void RemoveAssetsFrom_CPU_RAM();
	// Function to set the Texture Asset Manager Descriptor Heap.
	void SetDescriptorHeap(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList);
	// Function to apply the texture view to heaps that are to be set. (To be called by Graphics Engine).
	void SetAllTextureViewToRespectiveHeaps(Microsoft::WRL::ComPtr<ID3D12Device5> device);

private:

	struct TextureInfo
	{
		std::string fileName;
		UINT64 textureIndex;
	};
	std::vector<TextureInfo> loadedTextures = {};

	std::vector<ModelInfo> loadedModels = {};

	// Function to process each mesh from the model and submit it to Mesh Asset Manager and return the mesh index.
	UINT64 ProcessMesh(aiMesh* mesh);
	// Function to process the textures from the model, submit it to Texture Asset Manager and return the texture index.
	UINT64 ProcessColorTexture(aiMesh* mesh, const aiScene* scene, std::string modelFileLocation);
	// Function to process individual nodes of the mesh of the model.
	void ProcessNode(aiNode* node, const aiScene* scene, ModelInfo* modelInfo, std::string modelFileLocation);

public:

	// Function to import the model with mesh and assigned amterial textures.
	ModelInfo LoadModel(std::string fileLocation);
	// Function to set new mesh.
	UINT64 SetNewMesh(std::vector<MeshVertex> vertices, std::vector<unsigned short> indices);
	// Function to get the mesh from the Mesh Asset Manager.
	Mesh* GetMesh(UINT64 meshIndex);
	// Function to set a new texture.
	UINT64 SetNewTexture(std::string fileLocation);
	// Function to get the texture from Texture Asset Manager.
	Texture* GetTexture(UINT64 textureIndex);
	// Function to get texture GPU handle.
	D3D12_GPU_DESCRIPTOR_HANDLE GetTextureHandle(UINT64 textureIndex);

public:
	struct PreHandleStructure
	{
		UINT64 textureIndex;
		D3D12_CPU_DESCRIPTOR_HANDLE handle;
	};
	std::vector<PreHandleStructure> ToBeAppliedHandles;

	// Function to assign texture view to descriptor heap.
	void AssignTextureViewToHeap(UINT64 textureIndex, D3D12_CPU_DESCRIPTOR_HANDLE handle);
};