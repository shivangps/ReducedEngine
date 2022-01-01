#pragma once
#include "GraphicsHelper.h"

// This class defines the descriptor heap that will store the srv textures description and cbv data description for all. 

class UniversalDescriptorHeap
{
private:
	UniversalDescriptorHeap() {}

	// Heap that will store alll the view of shader resource as well as constant buffer view.
	DescriptorHeap srvCbvUavHeap = {};
	DescriptorHeap samplerHeap = {};

	// Maximum number of descriptors the heap can store.
	const UINT64 maxSrvCbvUavHeapCount = 1000000;
	const UINT64 maxSamplerHeapCount = 2048;

	// Currently number of descriptors stored in heap.
	UINT64 currentSrvCbvUavNum = 0;
	UINT64 currentSamplerNum = 0;

public:
	// Get a single instance of this static declared object.
	static UniversalDescriptorHeap* GetInstance()
	{
		static UniversalDescriptorHeap* instance = new UniversalDescriptorHeap();
		return instance;
	}

	// Function to initialize this descriptor heap.
	void Initialize(Microsoft::WRL::ComPtr<ID3D12Device5> device);

	// Function to set a new srv or cbv and get the index of the srv or sbc stored in the heap.
	UINT64 SetCpuHandle(Microsoft::WRL::ComPtr<ID3D12Device5> device, Microsoft::WRL::ComPtr<ID3D12Resource>* resource, D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc);
	UINT64 SetCpuHandle(Microsoft::WRL::ComPtr<ID3D12Device5> device, ID3D12Resource* resource, D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc);
	UINT64 SetCpuHandle(Microsoft::WRL::ComPtr<ID3D12Device5> device, D3D12_CONSTANT_BUFFER_VIEW_DESC* cbvDesc);
	UINT64 SetCpuHandle(Microsoft::WRL::ComPtr<ID3D12Device5> device, Microsoft::WRL::ComPtr<ID3D12Resource>* resource, Microsoft::WRL::ComPtr<ID3D12Resource>* pCounterResource, D3D12_UNORDERED_ACCESS_VIEW_DESC* uavDesc);
	UINT64 SetCpuHandle(Microsoft::WRL::ComPtr<ID3D12Device5> device, D3D12_SAMPLER_DESC* samplerDesc);

	// Function to get the srv or cbv gpu handle.
	D3D12_GPU_DESCRIPTOR_HANDLE GetCbvSrvUavGPUHandle(UINT64 index);
	D3D12_GPU_DESCRIPTOR_HANDLE GetSamplerGPUHandle(UINT64 index);

	 // Function to assign the heap to the command list.
	void AssignToGraphicsCommandList(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList);
};