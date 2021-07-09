#pragma once
#include <vector>
#include "Mesh.h"

// This header file contains a class to store single instances of meshes.
// As well as load it from a file.

class MeshAssetManager
{

	struct MeshAssetInfo
	{
		UINT64 meshIndex;
		Mesh mesh;
	};

	Microsoft::WRL::ComPtr<ID3D12Device5> graphicsDevice = nullptr;

	std::vector<MeshAssetInfo> meshes = {};
	UINT64 currentMeshIndex = 0;

	MeshAssetManager() {}

public:
	// Get single instance of the class.
	static MeshAssetManager* GetInstance()
	{
		static MeshAssetManager* instance = new MeshAssetManager();
		return instance;
	}

	// Function to set the device for loading all the mesh data to device(GPU) RAM.(To be called only by Graphics Engine)
	void SetDevice(Microsoft::WRL::ComPtr<ID3D12Device5> device);
	// Function to assign a new mesh and assigns as well as returns the index of the mesh.
	int SetNewMesh(std::vector<MeshVertex> vertices, std::vector<unsigned short> indices);
	// Function to call for loading all the meshes data to CPU RAM.
	void LoadAllMeshDataTo_GPU_RAM(Microsoft::WRL::ComPtr<ID3D12Device5> device, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList);
	// Function to call for removal of all meshes data from CPU RAM.
	void RemoveAllMeshDataFrom_CPU_RAM();
	// Function to get the mesh pointer base on mesh index entered.
	Mesh* GetMesh(UINT64 meshIndex);
};