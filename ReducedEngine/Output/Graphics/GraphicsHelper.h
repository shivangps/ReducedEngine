#pragma once
#include "d3dx12.h"
#include <dxgi1_4.h>

// This header file consists of all convinient functions and classes.

// Function to initialize resource.
void InitializeResource(Microsoft::WRL::ComPtr<ID3D12Resource>* resource, Microsoft::WRL::ComPtr<ID3D12Device5> device, D3D12_HEAP_PROPERTIES* heapProperties, D3D12_RESOURCE_DESC* resourceDesc, D3D12_RESOURCE_STATES resourceState, D3D12_CLEAR_VALUE* clearValue);

// Function to transition resource state.
void TransitionResourceState(Microsoft::WRL::ComPtr<ID3D12Resource> resource, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList,
	D3D12_RESOURCE_STATES beforeState, D3D12_RESOURCE_STATES afterState);

// Functiont otget the aggregate size of a constant buffer view.
unsigned int GetAggregateSize(unsigned int size);

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
	D3D12_DESCRIPTOR_HEAP_TYPE heapType = {};

public:
	// Initialization function.
	void Initialize(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int numberOfHeaps, D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flags);
	// Function to get the CPU handle offset from start of heap.
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(unsigned int offsetIndex);
	// Function to get the GPU handle offset from start of heap.
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(unsigned int offsetIndex);
	// Function to get heap size.
	unsigned int GetHeapSize();
	// Function to get heap type.
	D3D12_DESCRIPTOR_HEAP_TYPE GetHeapType();
	// Function to get the pointer of the heap.
	ID3D12DescriptorHeap* Get();
	// Function to release the pointer of the heap.
	void Release();
};

// Class to create and manage constant buffer.
class ConstantBuffer
{
private:
	bool initialized = false;
	Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
	D3D12_CONSTANT_BUFFER_VIEW_DESC constantBufferViewDesc = {};
	unsigned int size = 0;
	unsigned char* pointer = nullptr;

public:
	void Initialize(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int size, LPCWSTR name);
	D3D12_CONSTANT_BUFFER_VIEW_DESC* GetConstantBufferViewDesc();
	void CopyDataToConstantBufferLocation(const void* data);
	bool IsInitialized();
};

// Class to handle the graphics device package.
class GraphicsDeviceManager
{
private:
	// Contains info of all the graphics adapters attached to the system.
	Microsoft::WRL::ComPtr<IDXGIFactory4> factory = nullptr;
	// Contains the pointer to the graphics device.
	Microsoft::WRL::ComPtr<ID3D12Device5> device = nullptr;

	// Function to release the device and factory.
	void ReleaseDeviceAndFactory();

public:
	~GraphicsDeviceManager()
	{
		this->ReleaseDeviceAndFactory();
	}
	// Function to create and select the graphics device based on the amount of dedicated VRAM.
	void CreateDevice();
	// Function to get the device pointer.
	Microsoft::WRL::ComPtr<ID3D12Device5> GetDevice();
	// Function to create command queue.
	void CreateCommandQueue(Microsoft::WRL::ComPtr<ID3D12CommandQueue>* commandQueue, D3D12_COMMAND_LIST_TYPE commandListType);
	// Function to create swap chain.
	void CreateSwapChain(Microsoft::WRL::ComPtr<IDXGISwapChain3>* swapChain, Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue, HWND hWnd, unsigned int width, unsigned int height, unsigned int framesCount, DXGI_FORMAT swapChainFormat);
};

// Command list and command allocator for each thread.
struct CommandSet
{
	D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList = nullptr;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator = nullptr;

	void CreateSet(Microsoft::WRL::ComPtr<ID3D12Device5> device);
	void CreateSet(Microsoft::WRL::ComPtr<ID3D12Device5> device, std::string commandSetName);
	void CloseCommandList();
	void ResetCommandList();
	void ResetCommandAllocator();
	void ResetAll();
	void ReleaseAll();
};

// Function to create render targets.
void CreateRenderTargetsFromSwapChain(DescriptorHeap* heap, Microsoft::WRL::ComPtr<ID3D12Resource> renderTargetResource[], unsigned int numberOfRTBuffers, Microsoft::WRL::ComPtr<IDXGISwapChain3> swapChain, Microsoft::WRL::ComPtr<ID3D12Device5> device);
// Function to create depth stencil.
void CreateDepthStencil(DescriptorHeap* heap, Microsoft::WRL::ComPtr<ID3D12Resource>* depthStencilResource, DXGI_FORMAT depthStencilFormat, Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int width, unsigned int height);
// Function to create command allocator.
void CreateCommandAllocator(Microsoft::WRL::ComPtr<ID3D12CommandAllocator>* commandAllocator, Microsoft::WRL::ComPtr<ID3D12Device5> device, D3D12_COMMAND_LIST_TYPE commandListType);
// Function to create command list.
void CreateGraphicsCommandList(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4>* commandList, Microsoft::WRL::ComPtr<ID3D12Device5> device, Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator, D3D12_COMMAND_LIST_TYPE commandListType);
// Function to create fence.
void CreateFence(Microsoft::WRL::ComPtr<ID3D12Fence>* fence, UINT64* fenceValue, Microsoft::WRL::ComPtr<ID3D12Device5> device);
// Function to signal the new fence value.
void SignalFence(Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue, Microsoft::WRL::ComPtr<ID3D12Fence> fence, UINT64 fenceValue);
// Function to wait for GPU to reach the fence value.
void WaitForFenceValue(Microsoft::WRL::ComPtr<ID3D12Fence> fence, UINT64 fenceValue);
// Function to reset command list and command allocator.
void Reset(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList, Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator);
// Function to close the command list.
void CloseCommandList(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList);
// Function to present the frame.
void PresentFrame(Microsoft::WRL::ComPtr<IDXGISwapChain3> swapChain, unsigned int* currentFrame);
