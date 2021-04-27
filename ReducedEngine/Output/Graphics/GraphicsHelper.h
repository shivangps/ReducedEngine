#pragma once
#include "d3dx12.h"

// This header file consists of all convinient functions and classes.

// Function to initialize resource.
void InitializeResource(Microsoft::WRL::ComPtr<ID3D12Resource>* resource, Microsoft::WRL::ComPtr<ID3D12Device5> device, D3D12_HEAP_PROPERTIES* heapProperties, D3D12_RESOURCE_DESC* resourceDesc, D3D12_RESOURCE_STATES resourceState, D3D12_CLEAR_VALUE* clearValue);

// Function to transition resource state.
void TransitionResourceState(Microsoft::WRL::ComPtr<ID3D12Resource> resource, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList,
	D3D12_RESOURCE_STATES beforeState, D3D12_RESOURCE_STATES afterState);

// Class to manage descriptor heap.
class DescriptorHeap
{
public:
	DescriptorHeap() {}
	~DescriptorHeap()
	{
		mainHeap->Release();
	}

private:
	Microsoft::WRL::ComPtr<ID3D12Device5> device = nullptr;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mainHeap = nullptr;
	unsigned int heapSize = 0;
	unsigned int maximumHeaps = 0;
	D3D12_CPU_DESCRIPTOR_HANDLE startCPUHandle = {};
	D3D12_GPU_DESCRIPTOR_HANDLE startGPUHandle = {};

public:
	// Initialization function.
	void Initialize(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int numberOfHeaps, D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flags);
	// Function to get the CPU handle offset from start of heap.
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(unsigned int offsetIndex);
	// Function to get the GPU handle offset from start of heap.
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(unsigned int offsetIndex);
	// Function to get the pointer of the heap.
	ID3D12DescriptorHeap* Get();
};