#pragma once
#include "../../../Output/Graphics/2D/RenderComponent2D.h"
#include "../../../AssetManager/GameAssetManager.h"
#include "../../../Helper.h"

// Class that renders the 2D components in wireframe.

class WireframeRenderComponent2D : public RenderComponent2D
{
private:
	Shader* wireframeShader = nullptr;

	// Uninitialzed geometry data.
	struct Geometry2DData
	{
		std::vector<MeshVertex2D> vertices = {};
		std::vector<unsigned short> indices = {};
		std::string albedoTextureFileDirectory = "";
	};
	std::vector<Geometry2DData> geometryDataToBeLoaded = {};

	// Mesh data.
	struct Geometry2DCharacteristics
	{
		Mesh* assignedMesh;
		D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
		D3D12_INDEX_BUFFER_VIEW indexBufferView;
		unsigned int numberOfVertices;
		unsigned int numberOfIndices;
	};
	std::vector<Geometry2DCharacteristics> geometries = {};

	UINT64 albedoTextureIndex = 0;

	D3D12_GPU_DESCRIPTOR_HANDLE albedoTextureHandle = {};

	// Pixel data constant buffer.
	struct PixelData
	{
		Vector3 color;
		int Pad0 = 0;
	}pixelData;

	ConstantBuffer pixelDataConstantBuffer = {};
	D3D12_GPU_DESCRIPTOR_HANDLE pixelDataConstantBufferHandle = {};

	struct MatrixData
	{
		Matrix4 cameraMatrix;
	}matrixData;

	// Function to handle the constant buffer for matrix calculation.
	void InitializePixelDataConstantBuffer(Microsoft::WRL::ComPtr<ID3D12Device5> device, LPCWSTR constantBufferName);

public:
	WireframeRenderComponent2D(Transform2D* transform) : RenderComponent2D(transform) {}

	// Function to initialize this wireframe render component.
	void Initialize(Microsoft::WRL::ComPtr<ID3D12Device5> device);
	// Function to draw the render component in wireframe.
	void Draw(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList, Camera2D camera);
	// Function to load the 2D geometry predefined.
	void Load2DGeometry(std::vector<MeshVertex2D> vertices, std::vector<unsigned short> indices);
	// Function to change color of the wireframe model.
	void ChangeColor(Vector3 color);
};