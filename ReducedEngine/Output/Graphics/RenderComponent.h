#pragma once
#include "Camera.h"
#include "Shader.h"
#include "../AssetManager/GameAssetManager.h"
#include "GraphicsHelper.h"
#include "../../MathEngine.h"

// This header file contains the class definition for render components present in entities visible in the scene.

struct LocalData
{
	DirectX::XMFLOAT4X4 cameraMatrix;
};

class RenderComponent
{
private:
	// Uninitialzed data.
	struct MeshData
	{
		std::vector<MeshVertex> vertices = {};
		std::vector<unsigned short> indices = {};
		std::string colorTextureFileDirectory = "";
	};
	std::vector<MeshData> meshDataToBeLoaded = {};

	std::vector<std::string> modelFileLocation = {};

	Microsoft::WRL::ComPtr<ID3D12Resource> localDataResource = nullptr;

protected:

	Transform* transform = nullptr;

	Shader* shader = nullptr;

	struct MeshCharacteristics
	{
		Mesh* assignedMesh;
		D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
		D3D12_INDEX_BUFFER_VIEW indexBufferView;
		unsigned int numberOfVertices;
		unsigned int numberOfIndices;
	};
	std::vector<MeshCharacteristics> meshes = {};

	DescriptorHeap heap = {};

	UINT64 colorTextureIndex = 0;

	LocalData localData = {};
	unsigned char* pLocalDataCBV = nullptr;

	// Function to initialize the local data constant buffer.
	void InitializeLocalData(Microsoft::WRL::ComPtr<ID3D12Device5> device);
	// Function to calculate the matrix data from camera and load them to the local constant buffer.
	void UpdateCameraMatrix(Camera camera);

public:
	RenderComponent(Transform* transform) : transform(transform) {}

	// Function to initialize the vertices of a mesh along with indices.
	void InitializeNewMesh(std::vector<MeshVertex> vertices, std::vector<unsigned short> indices);
	void InitializeNewMesh(std::vector<MeshVertex> vertices, std::vector<unsigned short> indices, std::string colorTextureFileDirectory);
	// Function to initialize 3D models along with assigned textures.
	void LoadModel(std::string fileDirectory);
	// Function to set a new Shader to this rendering component.
	void SetShaderToComponent(Shader* shader);
	// Function to initialize the component with Graphics device.
	void InitializeComponent(Microsoft::WRL::ComPtr<ID3D12Device5> device, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList);
	// Function to draw the render component.
	void Draw(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList, Camera camera);
};