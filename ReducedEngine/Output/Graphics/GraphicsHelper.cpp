#include "GraphicsHelper.h"
#include "..//..//ErrorLogger.h"

void DescriptorHeap::Initialize(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int numberOfHeaps, D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flags)
{
	HRESULT HR;

	this->device = device;
	this->maximumHeaps = numberOfHeaps;

	// Describe descriptor heaps;
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.NumDescriptors = this->maximumHeaps;
	heapDesc.Type = type;
	heapDesc.Flags = flags;

	// Create the descriptor heap.
	HR = this->device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(this->mainHeap.GetAddressOf()));
	ExitOnError(HR, "Failed to create descriptor heap.");

	// Get heap size.
	this->heapSize = this->device->GetDescriptorHandleIncrementSize(type);

	// Get descriptor heap handle.
	this->startCPUHandle = this->mainHeap->GetCPUDescriptorHandleForHeapStart();
	this->startGPUHandle = this->mainHeap->GetGPUDescriptorHandleForHeapStart();
}

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::GetCPUHandle(unsigned int offsetIndex)
{
	D3D12_CPU_DESCRIPTOR_HANDLE handle;
	handle.ptr = this->startCPUHandle.ptr + (offsetIndex * heapSize);

	return handle;
}

D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeap::GetGPUHandle(unsigned int offsetIndex)
{
	D3D12_GPU_DESCRIPTOR_HANDLE handle;
	handle.ptr = this->startGPUHandle.ptr + (offsetIndex * heapSize);

	return handle;
}

ID3D12DescriptorHeap* DescriptorHeap::Get()
{
	return this->mainHeap.Get();
}

void InitializeResource(Microsoft::WRL::ComPtr<ID3D12Resource>* resource, Microsoft::WRL::ComPtr<ID3D12Device5> device, D3D12_HEAP_PROPERTIES* heapProperties, D3D12_RESOURCE_DESC* resourceDesc, D3D12_RESOURCE_STATES resourceState, D3D12_CLEAR_VALUE* clearValue)
{
	HRESULT HR;

	HR = device->CreateCommittedResource(
		heapProperties,
		D3D12_HEAP_FLAG_NONE,
		resourceDesc,
		resourceState,
		clearValue,
		IID_PPV_ARGS(resource->GetAddressOf()));
	ExitOnError(HR, "Failed to initialize resource.");
}

void TransitionResourceState(Microsoft::WRL::ComPtr<ID3D12Resource> resource, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList, D3D12_RESOURCE_STATES beforeState, D3D12_RESOURCE_STATES afterState)
{
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(resource.Get(), beforeState, afterState));
}

unsigned int GetAggregateSize(unsigned int size)
{
	return ((size + 255) & ~255);
}
