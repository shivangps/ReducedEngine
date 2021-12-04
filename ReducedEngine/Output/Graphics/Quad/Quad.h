#pragma once
#include "../MathEngine.h"
#include "../GraphicsHelper.h"

// // Class that draws a quad covering the whole screen for the post-processing effects.

class Quad
{
private:
	Quad() {}

	// Structure to store the format of the vertices for drawing the quad.
	struct QuadVertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 texCoord;

		QuadVertex(float x, float y, float z, float u, float v) : position(x, y, z), texCoord(u, v) {}
	};
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView = {};

public:
	// Function that returns a single instance of this object.
	static Quad* GetInstance()
	{
		static Quad* instance = new Quad();
		return instance;
	}

	// Function to initialize the quad geometry.
	void Initialize(Microsoft::WRL::ComPtr<ID3D12Device5> device);

	// Function to draw the quad on the screen.
	void Draw(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList);
};