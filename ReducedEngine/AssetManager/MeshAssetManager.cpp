#include "MeshAssetManager.h"

#include "../ErrorLogger.h"

void MeshAssetManager::SetDevice(Microsoft::WRL::ComPtr<ID3D12Device5> device)
{
	this->graphicsDevice = device;
}

int MeshAssetManager::SetNewMesh(std::vector<MeshVertex> vertices, std::vector<unsigned short> indices)
{
	if (this->graphicsDevice)
	{
		MeshAssetInfo meshInfo;

		meshInfo.meshIndex = this->currentMeshIndex++;
		meshInfo.mesh.Initialize(this->graphicsDevice, vertices, indices);

		this->meshes.push_back(meshInfo);

		return meshInfo.meshIndex;
	}
	else
	{
		ExitWithMessage("Graphics Device not assigned to Mesh Asset Manager.");
	}
	return -1;
}

void MeshAssetManager::LoadAllMeshDataTo_GPU_RAM(Microsoft::WRL::ComPtr<ID3D12Device5> device, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList)
{
	for (unsigned int i = 0; i < this->meshes.size(); i++)
	{
		this->meshes[i].mesh.LoadMeshDataTo_GPU_RAM(commandList);
	}
}

void MeshAssetManager::RemoveAllMeshDataFrom_CPU_RAM()
{
	for (unsigned int i = 0; i < this->meshes.size(); i++)
	{
		this->meshes[i].mesh.RemoveMeshDataFrom_CPU_RAM();
	}
}

Mesh* MeshAssetManager::GetMesh(UINT64 meshIndex)
{
	if(meshIndex <= this->currentMeshIndex)
		for (unsigned int i = 0; i < this->meshes.size(); i++)
		{
			if (meshIndex == this->meshes[i].meshIndex)
				return &this->meshes[i].mesh;
		}

	return nullptr;
}
