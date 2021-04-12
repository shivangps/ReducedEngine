#pragma once

// Graphics headers.
#include "..//..//CommonHeader.h"
#include "GraphicsHelper.h"
#include <dxgi1_4.h>
#include <wrl/client.h>

// Class that handles the rendering of objects present in the scene.

// This is a singleton.
class Graphics
{
	Graphics() {}

	HWND hWnd = NULL;

	D3D12_VIEWPORT viewport = {};
	D3D12_RECT clippingRect = {};

	Microsoft::WRL::ComPtr<IDXGIFactory4> factory = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Device5> device = nullptr;

	Microsoft::WRL::ComPtr<IDXGISwapChain3> swapChain = nullptr;
	DXGI_FORMAT renderTargetFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	unsigned int numberOfFrames = 2;

	D3D12_COMMAND_LIST_TYPE directCommandListType = D3D12_COMMAND_LIST_TYPE_DIRECT;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue = nullptr;
	UINT64 queueFenceValue = 0;
	Microsoft::WRL::ComPtr<ID3D12Fence> fence = nullptr;

	DescriptorHeap renderTargetHeap = {};
	static const unsigned int frameCount = 2;
	unsigned int currentFrame = 0;
	Microsoft::WRL::ComPtr<ID3D12Resource> renderTargetResource[frameCount] = {};

	DescriptorHeap depthStencilHeap = {};
	DXGI_FORMAT depthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource = nullptr;

	D3D12_COMMAND_LIST_TYPE directTypeList = D3D12_COMMAND_LIST_TYPE_DIRECT;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator = nullptr;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList = nullptr;
	UINT64 commandFenceValue = 0;

	// Function to create and select the graphics device based on the amount of dedicated VRAM.
	void CreateDevice();
	// Function to create command queue.
	void CreateCommandQueue(Microsoft::WRL::ComPtr<ID3D12CommandQueue>* commandQueue, Microsoft::WRL::ComPtr<ID3D12Device5> device, D3D12_COMMAND_LIST_TYPE commandListType);

	// Function to get queue fence value.
	UINT64 GetCurrentSetFence();

	// Function to create swap chain.
	void CreateSwapChain(Microsoft::WRL::ComPtr<IDXGISwapChain3>* swapChain, Microsoft::WRL::ComPtr<ID3D12Device5> device, Microsoft::WRL::ComPtr<IDXGIFactory4> factory, 
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue, HWND hWnd, unsigned int width, unsigned int height, unsigned int framesCount, DXGI_FORMAT swapChainFormat);
	// Function to create render targets.
	void CreateRenderTargets(DescriptorHeap* heap, Microsoft::WRL::ComPtr<ID3D12Resource> renderTargetResource[], unsigned int numberOfRTBuffers, Microsoft::WRL::ComPtr<IDXGISwapChain3> swapChain, Microsoft::WRL::ComPtr<ID3D12Device5> device);
	// Function to create depth stencil.
	void CreateDepthStencil(DescriptorHeap* heap, Microsoft::WRL::ComPtr<ID3D12Resource>* depthStencilResource, DXGI_FORMAT depthStencilFormat, Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int width, unsigned int height);

	// Function to create command allocator.
	void CreateCommandAllocator(Microsoft::WRL::ComPtr<ID3D12CommandAllocator>* commandAllocator, Microsoft::WRL::ComPtr<ID3D12Device5> device, D3D12_COMMAND_LIST_TYPE commandListType);
	// Function to create command list.
	void CreateGraphicsCommandList(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4>* commandList, UINT64* fenceValue, Microsoft::WRL::ComPtr<ID3D12Device5> device, Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator, D3D12_COMMAND_LIST_TYPE commandListType);
	// Function to create command list all together with command allocator.
	void CreateCommandListSet(Microsoft::WRL::ComPtr<ID3D12CommandAllocator>* commandAllocator, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4>* commandList, UINT64* fenceValue, Microsoft::WRL::ComPtr<ID3D12Device5> device, D3D12_COMMAND_LIST_TYPE commandListType);
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

public:
	// Function to get a single instance of graphics or render engine.
	static Graphics* GetInstance()
	{
		static Graphics* instance = new Graphics();
		return instance;
	}

	void Initialize(HWND windowHandle, unsigned int width, unsigned int height);
	void RenderScene();
};