#pragma once
#include "../CommonHeader.h"
#include "../Output/Graphics/d3dx12.h"

// Header file that contains the class definition for the geometry mesh of a 3d object.

struct MeshVertex
{
public:
	MeshVertex(float x, float y, float z, float nx, float ny, float nz, float u, float v, float tx, float ty, float tz, float bx, float by, float bz) : position(x, y, z), normal(nx, ny, nz), texCoord(u, v), tangent(tx, ty, tz), bitangent(bx, by, bz) {}
	MeshVertex(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 normal, DirectX::XMFLOAT2 texCoord, DirectX::XMFLOAT3 tangent, DirectX::XMFLOAT3 bitangent) : position(position), normal(normal), texCoord(texCoord), tangent(tangent), bitangent(bitangent)  {}

	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT2 texCoord;
	DirectX::XMFLOAT3 tangent;
	DirectX::XMFLOAT3 bitangent;
};

struct MeshVertex2D
{
public:
	MeshVertex2D(float x, float y, float u, float v) : position(x, y), texCoord(u, y) {}

	DirectX::XMFLOAT2 position;
	DirectX::XMFLOAT2 texCoord;
};

class Mesh
{
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexUploadBuffer = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexDefaultBuffer = nullptr;
	unsigned int numberOfVertices = 0;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView = {};
	unsigned int numberOfIndices = 0;
	D3D12_INDEX_BUFFER_VIEW indexBufferView = {};
	std::vector<unsigned short> indices = {};
	D3D12_SUBRESOURCE_DATA indexData = {};

public:
	Mesh() {}

	// Initialize the mesh by creating the resources necessary for rendering.
	void Initialize(Microsoft::WRL::ComPtr<ID3D12Device5> device, std::vector<MeshVertex> vertices, std::vector<unsigned short> indices);
	// Function to load the mesh resource to GPU RAM.
	void LoadMeshDataTo_GPU_RAM(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList);
	// Function to remove mesh data from CPU RAM.
	void RemoveMeshDataFrom_CPU_RAM();
	// Function to call for drawing the mesh.
	void Draw(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList);
	// Function to get Vertex Buffer View.
	D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView();
	// Function to get Index Buffer View.
	D3D12_INDEX_BUFFER_VIEW GetIndexBufferView();
	// Function to get number of vertices.
	unsigned int GetNumberOfVertices();
	// Function to get number of indices.
	unsigned int GetNumberOfIndices();
	// Function to release all the vertex and index resources.
	void ReleaseVertexData();

public:
	// Initialize the 2D mesh and its resources.
	void Initialize2D(Microsoft::WRL::ComPtr<ID3D12Device5> device, std::vector<MeshVertex2D> vertices, std::vector<unsigned short> indices);
};