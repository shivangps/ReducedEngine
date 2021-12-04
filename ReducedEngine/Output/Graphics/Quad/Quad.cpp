#include "Quad.h"

void Quad::Initialize(Microsoft::WRL::ComPtr<ID3D12Device5> device)
{
	QuadVertex vertices[] =
	{
		QuadVertex(-1.0f, 1.0f, 0.0f, 0.0f, 1.0f),
		QuadVertex(-1.0f,-1.0f, 0.0f, 0.0f, 0.0f),
		QuadVertex(1.0f, 1.0f, 0.0f, 1.0f, 1.0f),
		QuadVertex(1.0f,-1.0f, 0.0f, 1.0f, 0.0f),
	};
	unsigned int quadBufferSize = sizeof(vertices);

	// Create the vertex buffer resource for quad.
	InitializeResource(&this->vertexBuffer, device, &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), &CD3DX12_RESOURCE_DESC::Buffer(quadBufferSize), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr);
	this->vertexBuffer->SetName(L"Quad Vertex Upload Buffer");

	// Fill the quad vertex data.
	unsigned char* pQuadVertexDataBegin;
	CD3DX12_RANGE readRange(0, 0);
	this->vertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pQuadVertexDataBegin));
	memcpy(pQuadVertexDataBegin, vertices, quadBufferSize);
	this->vertexBuffer->Unmap(0, nullptr);

	// Initialize vertex buffer view.
	this->vertexBufferView.BufferLocation = this->vertexBuffer->GetGPUVirtualAddress();
	this->vertexBufferView.SizeInBytes = quadBufferSize;
	this->vertexBufferView.StrideInBytes = sizeof(QuadVertex);
}

void Quad::Draw(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList)
{
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	commandList->IASetVertexBuffers(0, 1, &this->vertexBufferView);
	commandList->DrawInstanced(4, 1, 0, 0);
}
