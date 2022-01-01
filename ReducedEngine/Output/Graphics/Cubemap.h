#pragma once
#include "../MathEngine.h"
#include "../../AssetManager/Texture.h"
#include "Shader.h"
#include "Camera.h"
#include "../ErrorLogger.h"
#include <DirectXTK/DDSTextureLoader.h>
#include "UniversalDescriptorHeap.h"

struct TransformMatrix
{
	Matrix4 model;
	Matrix4 projection;
	Matrix4 view;
	Matrix4 viewProj;
};

struct SkyboxVertex
{
public:
	SkyboxVertex(float x, float y, float z) : position(x, y, z) {}

	DirectX::XMFLOAT3 position;
};

class Cubemap
{
public:
	Cubemap() {}
	~Cubemap() {}

	UniversalDescriptorHeap* universalDescriptorHeap = UniversalDescriptorHeap::GetInstance();

	Microsoft::WRL::ComPtr<ID3D12Resource> textureResource = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> uploadResource = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> constantResource = nullptr;
	TransformMatrix matrix = {};
	UINT8* pCBVMatrix = nullptr;

	D3D12_GPU_DESCRIPTOR_HANDLE textureHandle = {};
	D3D12_GPU_DESCRIPTOR_HANDLE constantBufferHandle = {};

	Shader* shader = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> VertexUploadBuffer = nullptr;
	D3D12_VERTEX_BUFFER_VIEW VertexBufferView;
	UINT NumVertices = 0;

	void Initialize(Microsoft::WRL::ComPtr<ID3D12Device5> device, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList, std::string skymapFileDirectory);

	void Draw(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList, Camera camera);
};