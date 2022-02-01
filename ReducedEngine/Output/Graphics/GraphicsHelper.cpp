#include "GraphicsHelper.h"
#include "..//..//ErrorLogger.h"

void DescriptorHeap::Initialize(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int numberOfHeaps, D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flags)
{
	HRESULT HR;

	this->device = device;
	this->maximumHeaps = numberOfHeaps;
	this->heapType = type;

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

unsigned int DescriptorHeap::GetHeapSize()
{
	return this->heapSize;
}

D3D12_DESCRIPTOR_HEAP_TYPE DescriptorHeap::GetHeapType()
{
	return this->heapType;
}

ID3D12DescriptorHeap* DescriptorHeap::Get()
{
	return this->mainHeap.Get();
}

void DescriptorHeap::Release()
{
	this->mainHeap->Release();
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

void CommandSet::CreateSet(Microsoft::WRL::ComPtr<ID3D12Device5> device)
{
	HRESULT HR;

	HR = device->CreateCommandAllocator(this->type, IID_PPV_ARGS(this->commandAllocator.GetAddressOf()));
	ExitOnError(HR, "Failed to create command allocator.");

	HR = device->CreateCommandList(0, this->type, this->commandAllocator.Get(), nullptr, IID_PPV_ARGS(this->commandList.GetAddressOf()));
	ExitOnError(HR, "Failed to create graphics command list.");
}

void CommandSet::CreateSet(Microsoft::WRL::ComPtr<ID3D12Device5> device, std::string commandSetName)
{
	HRESULT HR;

	HR = device->CreateCommandAllocator(this->type, IID_PPV_ARGS(this->commandAllocator.GetAddressOf()));
	ExitOnError(HR, "Failed to create command allocator.");
	this->commandAllocator->SetName(StringToWide(commandSetName + " Command Allocator").c_str());

	HR = device->CreateCommandList(0, this->type, this->commandAllocator.Get(), nullptr, IID_PPV_ARGS(this->commandList.GetAddressOf()));
	ExitOnError(HR, "Failed to create graphics command list.");
	this->commandList->SetName(StringToWide(commandSetName + " Command List").c_str());
}

void CommandSet::CloseCommandList()
{
	HRESULT HR;

	HR = this->commandList->Close();
	ExitOnError(HR, "Failed to close the command list.");
}

void CommandSet::ResetCommandList()
{
	HRESULT HR;

	HR = this->commandList->Reset(this->commandAllocator.Get(), nullptr);
	ExitOnError(HR, "Failed to reset command list.");
}

void CommandSet::ResetCommandAllocator()
{
	HRESULT HR;

	HR = this->commandAllocator->Reset();
	ExitOnError(HR, "Failed to reset command allocator.");
}

void CommandSet::ResetAll()
{
	HRESULT HR;

	HR = this->commandAllocator->Reset();
	ExitOnError(HR, "Failed to reset command allocator.");

	HR = this->commandList->Reset(this->commandAllocator.Get(), nullptr);
	ExitOnError(HR, "Failed to reset command list.");
}

void CommandSet::ReleaseAll()
{
	this->commandList->Release();
	this->commandAllocator->Release();
}

void CreateRenderTargetsFromSwapChain(DescriptorHeap* heap, Microsoft::WRL::ComPtr<ID3D12Resource> renderTargetResource[], unsigned int numberOfRTBuffers, Microsoft::WRL::ComPtr<IDXGISwapChain3> swapChain, Microsoft::WRL::ComPtr<ID3D12Device5> device)
{
	HRESULT HR;

	// Create render target view descriptor heap.
	heap->Initialize(device, numberOfRTBuffers, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);

	// Create render target view.
	for (unsigned int i = 0; i < numberOfRTBuffers; i++)
	{
		HR = swapChain->GetBuffer(i, IID_PPV_ARGS(renderTargetResource[i].GetAddressOf()));
		device->CreateRenderTargetView(renderTargetResource[i].Get(), nullptr, heap->GetCPUHandle(i));
		renderTargetResource[i]->SetName(L" Back Buffer " + i);
	}
}

void CreateDepthStencil(DescriptorHeap* heap, Microsoft::WRL::ComPtr<ID3D12Resource>* depthStencilResource, DXGI_FORMAT depthStencilFormat, Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int width, unsigned int height)
{
	// Create descriptor heap.
	heap->Initialize(device, 1, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);

	// Clear value description.
	D3D12_CLEAR_VALUE depthStencilClearValue = {};
	depthStencilClearValue.Format = depthStencilFormat;
	depthStencilClearValue.DepthStencil.Depth = 1.0f;
	depthStencilClearValue.DepthStencil.Stencil = 0;

	// Describe the depth stencil buffer.
	D3D12_RESOURCE_DESC depthStencilDesc = {};
	depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthStencilDesc.Alignment = 0;
	depthStencilDesc.Width = width;
	depthStencilDesc.Height = height;
	depthStencilDesc.DepthOrArraySize = 1;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.Format = depthStencilFormat;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	// Create depth stencil resource.
	InitializeResource(depthStencilResource, device, &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), &depthStencilDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &depthStencilClearValue);

	// Create depth stencil view.
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = depthStencilFormat;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	dsvDesc.Texture2D.MipSlice = 0;

	// Create depth stencil view.
	device->CreateDepthStencilView(depthStencilResource->Get(), &dsvDesc, heap->GetCPUHandle(0));
}

void CreateCommandAllocator(Microsoft::WRL::ComPtr<ID3D12CommandAllocator>* commandAllocator, Microsoft::WRL::ComPtr<ID3D12Device5> device, D3D12_COMMAND_LIST_TYPE commandListType)
{
	HRESULT HR;

	HR = device->CreateCommandAllocator(commandListType, IID_PPV_ARGS(commandAllocator->GetAddressOf()));
	ExitOnError(HR, "Failed to create command allocator.");
}

void CreateGraphicsCommandList(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4>* commandList, Microsoft::WRL::ComPtr<ID3D12Device5> device, Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator, D3D12_COMMAND_LIST_TYPE commandListType)
{
	HRESULT HR;

	HR = device->CreateCommandList(0, commandListType, commandAllocator.Get(), nullptr, IID_PPV_ARGS(commandList->GetAddressOf()));
	ExitOnError(HR, "Failed to create graphics command list.");
}

void CreateFence(Microsoft::WRL::ComPtr<ID3D12Fence>* fence, UINT64* fenceValue, Microsoft::WRL::ComPtr<ID3D12Device5> device)
{
	HRESULT HR;

	*fenceValue = 0;
	HR = device->CreateFence(*fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence->GetAddressOf()));
	ExitOnError(HR, "Failed to create fence.");
}

void SignalFence(Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue, Microsoft::WRL::ComPtr<ID3D12Fence> fence, UINT64 fenceValue)
{
	HRESULT HR;

	HR = commandQueue->Signal(fence.Get(), fenceValue);
	ExitOnError(HR, "Failed to signal fence.");
}

void WaitForFenceValue(Microsoft::WRL::ComPtr<ID3D12Fence> fence, UINT64 fenceValue)
{
	HRESULT HR;
	HANDLE fenceEvent = NULL;

	if (fence->GetCompletedValue() < fenceValue)
	{
		HR = fence->SetEventOnCompletion(fenceValue, fenceEvent);
		ExitOnError(HR, "Failed to set fence event.");

		WaitForSingleObject(fenceEvent, INFINITE);
	}
}

void Reset(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList, Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator)
{
	HRESULT HR;

	HR = commandAllocator->Reset();
	ExitOnError(HR, "Failed to reset command allocator.");

	HR = commandList->Reset(commandAllocator.Get(), nullptr);
	ExitOnError(HR, "Failed to reset command list.");
}

void CloseCommandList(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList)
{
	HRESULT HR;

	HR = commandList->Close();
	ExitOnError(HR, "Failed to close the command list.");
}

void PresentFrame(Microsoft::WRL::ComPtr<IDXGISwapChain3> swapChain, unsigned int* currentFrame)
{
	HRESULT HR;

	HR = swapChain->Present(1, 0);
	ExitOnError(HR, "Failed to present frame.");

	*currentFrame = swapChain->GetCurrentBackBufferIndex();
}

void ConstantBuffer::Initialize(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int size, LPCWSTR name)
{
	// Reassign the size for the aggregate size of the constant buffer.
	this->size = GetAggregateSize(size);

	// Initialize the constant buffer.
	InitializeResource(&this->resource, device, &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), &CD3DX12_RESOURCE_DESC::Buffer(this->size), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr);
	this->resource->SetName(name);

	// Create the constant buffer view.
	this->constantBufferViewDesc.BufferLocation = this->resource->GetGPUVirtualAddress();
	this->constantBufferViewDesc.SizeInBytes = this->size;

	// Get the pointer location for copying the data to constant buffer.
	CD3DX12_RANGE readRange = {};
	this->resource->Map(0, &readRange, reinterpret_cast<void**>(&this->pointer));
	this->resource->Unmap(0, nullptr);

	// Set initialized boolean to true.
	this->initialized = true;
}

D3D12_CONSTANT_BUFFER_VIEW_DESC* ConstantBuffer::GetConstantBufferViewDesc()
{
	return &this->constantBufferViewDesc;
}

void ConstantBuffer::CopyDataToConstantBufferLocation(const void* data)
{
	memcpy(this->pointer, data, this->size);
}

bool ConstantBuffer::IsInitialized()
{
	return this->initialized;
}

void GraphicsDeviceManager::ReleaseDeviceAndFactory()
{
	this->device->Release();
	this->factory->Release();
}

void GraphicsDeviceManager::CreateDevice()
{
	HRESULT HR;

	// Create device factory.
	unsigned int FactoryFlags = 0;

#if defined(_DEBUG)
	Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		debugController->EnableDebugLayer();
		FactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
	}
#endif

	HR = CreateDXGIFactory2(FactoryFlags, IID_PPV_ARGS(this->factory.GetAddressOf()));
	ExitOnError(HR, "Failed to create factory.");

	// Select a suitable adapter(a.k.a GPU) to process graphics.
	std::vector<Microsoft::WRL::ComPtr<IDXGIAdapter1>> adapters;
	std::vector<UINT64> vrams;
	Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter = nullptr;
	unsigned int index = 0;
	// Get all the adapters present.
	while (SUCCEEDED(this->factory->EnumAdapters1(index++, adapter.GetAddressOf())))
	{
		adapters.push_back(adapter);
		// Get adapter VRAM details.
		DXGI_ADAPTER_DESC1 adapterDesc = {};
		adapter->GetDesc1(&adapterDesc);
		vrams.push_back(adapterDesc.DedicatedVideoMemory);
	}
	// Select the adapter with the highest VRAM.
	UINT64 MAX_VRAM = 0;
	unsigned int suitableAdapterIndex = 0;
	for (unsigned int i = 0; i < vrams.size(); i++)
	{
		if (MAX_VRAM < vrams[i])
		{
			MAX_VRAM = vrams[i];
			suitableAdapterIndex = i;
		}
	}

	// Create device from the selected high performance graphics adapter.
	Microsoft::WRL::ComPtr<IDXGIAdapter1> selectedAdapter = adapters[suitableAdapterIndex];

	HR = D3D12CreateDevice(selectedAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(this->device.GetAddressOf()));
	ExitOnError(HR, "Failed to select and create render device.");
}

Microsoft::WRL::ComPtr<ID3D12Device5> GraphicsDeviceManager::GetDevice()
{
	return this->device;
}

void GraphicsDeviceManager::CreateCommandQueue(Microsoft::WRL::ComPtr<ID3D12CommandQueue>* commandQueue, D3D12_COMMAND_LIST_TYPE commandListType)
{
	HRESULT HR;

	// Command queue creation.
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = commandListType;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	HR = this->device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(commandQueue->GetAddressOf()));
	ExitOnError(HR, "Failed to create command queue.");
}

void GraphicsDeviceManager::CreateSwapChain(Microsoft::WRL::ComPtr<IDXGISwapChain3>* swapChain, Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue, HWND hWnd, unsigned int width, unsigned int height, unsigned int framesCount, DXGI_FORMAT swapChainFormat)
{
	HRESULT HR;

	// Describe swap chain.
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.BufferCount = framesCount;
	swapChainDesc.Width = width;
	swapChainDesc.Height = height;
	swapChainDesc.Format = swapChainFormat;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	swapChainDesc.SampleDesc.Count = 1;

	// Create the swap chain.
	Microsoft::WRL::ComPtr<IDXGISwapChain1> tSwapChain = nullptr;
	HR = this->factory->CreateSwapChainForHwnd(commandQueue.Get(), hWnd, &swapChainDesc, nullptr, nullptr, &tSwapChain);
	ExitOnError(HR, "Failed to create swap chain.");

	HR = tSwapChain.As(swapChain);
	ExitOnError(HR, "Failed to transfer swap chain.");

	// Disable the fullscale toggle feature.
	HR = this->factory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER);
	ExitOnError(HR, "Failed to disable fullscreen toggle feature");
}
