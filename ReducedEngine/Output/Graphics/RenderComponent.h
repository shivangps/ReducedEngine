#pragma once
#include "Camera.h"
#include "Shader.h"
#include "../AssetManager/GameAssetManager.h"
#include "GraphicsHelper.h"
#include "../../MathEngine.h"
#include "../Helper.h"

// This header file contains the class definition for render components present in entities visible in the scene.

struct ShadowData
{
	Matrix4 lightSpaceMatrix;
};

struct LocalData
{
	Matrix4 cameraMatrix;
	Matrix4 modelMatrix;
	Matrix4 viewMatrix;
	Matrix4 normalMatrix;
	Matrix4 modelViewMatrix;
};

class RenderComponent
{
private:
	UniversalDescriptorHeap* universalDescHeap = UniversalDescriptorHeap::GetInstance();

	// Boolean to keep track if the render component is to be rendered or not.
	Boolean* enable = nullptr;

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

	enum SLOT
	{
		local_data_cbv = 0,
		color_texture_srv,
		normal_texture_srv,
		size
	};

	D3D12_GPU_DESCRIPTOR_HANDLE localDataCbvHandle = {};
	D3D12_GPU_DESCRIPTOR_HANDLE colorTextureHandle = {};
	D3D12_GPU_DESCRIPTOR_HANDLE normalTextureHandle = {};

protected:
	// To store the pointer of the transform of its corresponding gameobject.
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

	UINT64 colorTextureIndex = 0;
	UINT64 normalTextureIndex = 0;

	LocalData localData = {};
	unsigned char* pLocalDataCBV = nullptr;

	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> bundleListAllocator = nullptr;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> bundleList = nullptr;

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
	// Function to set the bundle command list render call.
	void InitializeBundleListForRender();

private:
	// Shadow depth render.
	ShadowData shadowData = {};

	D3D12_GPU_DESCRIPTOR_HANDLE shadowCbvHandle = {};

	Microsoft::WRL::ComPtr<ID3D12Resource> shadowDataResource = nullptr;

	unsigned char* pShadowDataCBV = nullptr;

public:
	// Function to initialize the constant buffer for shadows.
	void InitializeShadowConstantBuffer(Microsoft::WRL::ComPtr<ID3D12Device5> device);
	// Function to draw the component for shadows. (without traditional shaders or textures);
	void DrawForShadow(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList, Matrix4 lightSpaceMatrix);
	// Function to assign pointer to the enble bool variable.
	void SetEnable(Boolean* pBool);
	// Funtion to enable or disable rendering of the component in the list.
	void EnableRendering();
	void DisableRendering();
};