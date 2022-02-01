#include "Mesh.h"
#include "../Output/Graphics/GraphicsHelper.h"

void Mesh::Initialize(Microsoft::WRL::ComPtr<ID3D12Device5> device, std::vector<MeshVertex> vertices, std::vector<unsigned short> indices)
{
	// VERTEX
	numberOfVertices = vertices.size();

	unsigned int sizeOfVertices = sizeof(MeshVertex) * this->numberOfVertices;

	// Create vertex buffer to store the vertex in GPU memory.
	InitializeResource(&this->vertexResource, device, &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), &CD3DX12_RESOURCE_DESC::Buffer(sizeOfVertices), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr);
	this->vertexResource->SetName(L"Mesh Vertex Resource");

	// Storing the vertex data in the buffer.
	unsigned char* pMeshVertexDataBegin = nullptr;
	CD3DX12_RANGE readRange(0, 0);
	this->vertexResource->Map(0, &readRange, reinterpret_cast<void**>(&pMeshVertexDataBegin));
	memcpy(pMeshVertexDataBegin, &vertices[0], sizeOfVertices);
	this->vertexResource->Unmap(0, nullptr);

	// Define the vertex buffer in a view;
	this->vertexBufferView.BufferLocation = this->vertexResource->GetGPUVirtualAddress();
	this->vertexBufferView.SizeInBytes = sizeOfVertices;
	this->vertexBufferView.StrideInBytes = sizeof(MeshVertex);

	// INDEX
	this->numberOfIndices = indices.size();

	unsigned int sizeOfIndices = sizeof(unsigned short) * this->numberOfIndices;

	// Create default index buffer.
	InitializeResource(&this->indexDefaultBuffer, device, &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), &CD3DX12_RESOURCE_DESC::Buffer(sizeOfIndices), D3D12_RESOURCE_STATE_COPY_DEST, nullptr);
	this->indexDefaultBuffer->SetName(L"Mesh Index Default Buffer");

	// Create an upload buffer to upload the index to GPU memory.
	InitializeResource(&this->indexUploadBuffer, device, &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), &CD3DX12_RESOURCE_DESC::Buffer(sizeOfVertices), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr);
	this->indexUploadBuffer->SetName(L"Mesh Index Upload Buffer");

	this->indices = indices;
	this->indexData.RowPitch = sizeOfIndices;
	this->indexData.SlicePitch = sizeOfIndices;

	// Define the index buffer in a view.
	this->indexBufferView.BufferLocation = this->indexDefaultBuffer->GetGPUVirtualAddress();
	this->indexBufferView.Format = DXGI_FORMAT_R16_UINT;
	this->indexBufferView.SizeInBytes = sizeOfIndices;
}

void Mesh::LoadMeshDataTo_GPU_RAM(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList)
{
	this->indexData.pData = reinterpret_cast<BYTE*>(&this->indices[0]);
	// Copy the contents of subresource to another buffer.
	UpdateSubresources(commandList.Get(), this->indexDefaultBuffer.Get(), this->indexUploadBuffer.Get(), 0, 0, 1, &indexData);

	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(this->indexDefaultBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));
}

void Mesh::RemoveMeshDataFrom_CPU_RAM()
{
	this->indices.clear();
}

void Mesh::Draw(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList)
{
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->IASetVertexBuffers(0, 1, &this->vertexBufferView);
	commandList->IASetIndexBuffer(&this->indexBufferView);
	commandList->DrawIndexedInstanced(this->numberOfIndices, 1, 0, 0, 0);
}

D3D12_VERTEX_BUFFER_VIEW Mesh::GetVertexBufferView()
{
	return this->vertexBufferView;
}

D3D12_INDEX_BUFFER_VIEW Mesh::GetIndexBufferView()
{
	return this->indexBufferView;
}

unsigned int Mesh::GetNumberOfVertices()
{
	return this->numberOfVertices;
}

unsigned int Mesh::GetNumberOfIndices()
{
	return this->numberOfIndices;
}

void Mesh::ReleaseVertexData()
{
	this->vertexResource->Release();
	this->indexDefaultBuffer->Release();
	this->indexUploadBuffer->Release();
}

void Mesh::Initialize2D(Microsoft::WRL::ComPtr<ID3D12Device5> device, std::vector<MeshVertex2D> vertices, std::vector<unsigned short> indices)
{
	// VERTEX
	numberOfVertices = vertices.size();

	unsigned int sizeOfVertices = sizeof(MeshVertex2D) * this->numberOfVertices;

	// Create vertex buffer to store the vertex in GPU memory.
	InitializeResource(&this->vertexResource, device, &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), &CD3DX12_RESOURCE_DESC::Buffer(sizeOfVertices), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr);
	this->vertexResource->SetName(L"Mesh Vertex 2D Resource");

	// Storing the vertex data in the buffer.
	unsigned char* pMeshVertexDataBegin = nullptr;
	CD3DX12_RANGE readRange(0, 0);
	this->vertexResource->Map(0, &readRange, reinterpret_cast<void**>(&pMeshVertexDataBegin));
	memcpy(pMeshVertexDataBegin, &vertices[0], sizeOfVertices);
	this->vertexResource->Unmap(0, nullptr);

	// Define the vertex buffer in a view;
	this->vertexBufferView.BufferLocation = this->vertexResource->GetGPUVirtualAddress();
	this->vertexBufferView.SizeInBytes = sizeOfVertices;
	this->vertexBufferView.StrideInBytes = sizeof(MeshVertex2D);

	// INDEX
	this->numberOfIndices = indices.size();

	unsigned int sizeOfIndices = sizeof(unsigned short) * this->numberOfIndices;

	// Create default index buffer.
	InitializeResource(&this->indexDefaultBuffer, device, &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), &CD3DX12_RESOURCE_DESC::Buffer(sizeOfIndices), D3D12_RESOURCE_STATE_COPY_DEST, nullptr);
	this->indexDefaultBuffer->SetName(L"Mesh Index 2D Default Buffer");

	// Create an upload buffer to upload the index to GPU memory.
	InitializeResource(&this->indexUploadBuffer, device, &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), &CD3DX12_RESOURCE_DESC::Buffer(sizeOfVertices), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr);
	this->indexUploadBuffer->SetName(L"Mesh Index 2D Upload Buffer");

	this->indices = indices;
	this->indexData.RowPitch = sizeOfIndices;
	this->indexData.SlicePitch = sizeOfIndices;

	// Define the index buffer in a view.
	this->indexBufferView.BufferLocation = this->indexDefaultBuffer->GetGPUVirtualAddress();
	this->indexBufferView.Format = DXGI_FORMAT_R16_UINT;
	this->indexBufferView.SizeInBytes = sizeOfIndices;
}
