#include "d3dx12.h"
#include "..//..//Helper.h"
#include "Graphics.h"
#include "..//..//Assets/DisplayShader/DisplayShader.h"
#include "..//..//Assets/DirectionalLightShader/DirectionalLightShader.h"

void Graphics::CreateDevice()
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

void Graphics::CreateCommandQueue(Microsoft::WRL::ComPtr<ID3D12CommandQueue>* commandQueue, Microsoft::WRL::ComPtr<ID3D12Device5> device, D3D12_COMMAND_LIST_TYPE commandListType)
{
	HRESULT HR;

	// Command queue creation.
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = commandListType;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	HR = device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(commandQueue->GetAddressOf()));
	ExitOnError(HR, "Failed to create command queue.");
}

UINT64 Graphics::GetCurrentSetFence()
{
	return ++this->queueFenceValue;
}

void Graphics::CreateSwapChain(Microsoft::WRL::ComPtr<IDXGISwapChain3>* swapChain, Microsoft::WRL::ComPtr<ID3D12Device5> device, Microsoft::WRL::ComPtr<IDXGIFactory4> factory,
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue, HWND hWnd,
	unsigned int width, unsigned int height, unsigned int framesCount, DXGI_FORMAT swapChainFormat)
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
	HR = factory->CreateSwapChainForHwnd(commandQueue.Get(), hWnd, &swapChainDesc, nullptr, nullptr, &tSwapChain);
	ExitOnError(HR, "Failed to create swap chain.");

	HR = tSwapChain.As(swapChain);
	ExitOnError(HR, "Failed to transfer swap chain.");

	// Disable the fullscale toggle feature.
	HR = factory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER);
	ExitOnError(HR, "Failed to disable fullscreen toggle feature");
}

void Graphics::CreateRenderTargets(DescriptorHeap* heap, Microsoft::WRL::ComPtr<ID3D12Resource> renderTargetResource[], unsigned int numberOfRTBuffers, Microsoft::WRL::ComPtr<IDXGISwapChain3> swapChain, Microsoft::WRL::ComPtr<ID3D12Device5> device)
{
	HRESULT HR;

	// Create render target view descriptor heap.
	heap->Initialize(device, numberOfRTBuffers, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);

	// Create render target view.
	for (unsigned int i = 0; i < numberOfRTBuffers; i++)
	{
		HR = this->swapChain->GetBuffer(i, IID_PPV_ARGS(renderTargetResource[i].GetAddressOf()));
		device->CreateRenderTargetView(renderTargetResource[i].Get(), nullptr, heap->GetCPUHandle(i));
		renderTargetResource[i]->SetName(L" Back Buffer " + i);
	}
}

void Graphics::CreateDepthStencil(DescriptorHeap* heap, Microsoft::WRL::ComPtr<ID3D12Resource>* depthStencilResource, DXGI_FORMAT depthStencilFormat, Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int width, unsigned int height)
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

void Graphics::CreateCommandAllocator(Microsoft::WRL::ComPtr<ID3D12CommandAllocator>* commandAllocator, Microsoft::WRL::ComPtr<ID3D12Device5> device, D3D12_COMMAND_LIST_TYPE commandListType)
{
	HRESULT HR;

	HR = device->CreateCommandAllocator(commandListType, IID_PPV_ARGS(commandAllocator->GetAddressOf()));
	ExitOnError(HR, "Failed to create command allocator.");
}

void Graphics::CreateGraphicsCommandList(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4>* commandList, UINT64* fenceValue, Microsoft::WRL::ComPtr<ID3D12Device5> device, Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator, D3D12_COMMAND_LIST_TYPE commandListType)
{
	HRESULT HR;

	HR = device->CreateCommandList(0, commandListType, commandAllocator.Get(), nullptr, IID_PPV_ARGS(commandList->GetAddressOf()));
	ExitOnError(HR, "Failed to create graphics command list.");

	*fenceValue = this->GetCurrentSetFence();
}

void Graphics::CreateCommandListSet(Microsoft::WRL::ComPtr<ID3D12CommandAllocator>* commandAllocator, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4>* commandList, UINT64* fenceValue, Microsoft::WRL::ComPtr<ID3D12Device5> device, D3D12_COMMAND_LIST_TYPE commandListType)
{
	this->CreateCommandAllocator(commandAllocator, device, commandListType);
	this->CreateGraphicsCommandList(commandList, fenceValue, device, *commandAllocator, commandListType);
}

void Graphics::CreateFence(Microsoft::WRL::ComPtr<ID3D12Fence>* fence, UINT64* fenceValue, Microsoft::WRL::ComPtr<ID3D12Device5> device)
{
	HRESULT HR;

	*fenceValue = 0;
	HR = device->CreateFence(*fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence->GetAddressOf()));
	ExitOnError(HR, "Failed to create fence.");
}

void Graphics::SignalFence(Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue, Microsoft::WRL::ComPtr<ID3D12Fence> fence, UINT64 fenceValue)
{
	HRESULT HR;

	HR = commandQueue->Signal(fence.Get(), fenceValue);
	ExitOnError(HR, "Failed to signal fence.");
}

void Graphics::WaitForFenceValue(Microsoft::WRL::ComPtr<ID3D12Fence> fence, UINT64 fenceValue)
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

void Graphics::Reset(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList, Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator)
{
	HRESULT HR;

	HR = commandAllocator->Reset();
	ExitOnError(HR, "Failed ot reset command allocator.");

	HR = commandList->Reset(commandAllocator.Get(), nullptr);
	ExitOnError(HR, "Failed to reset command list.");
}

void Graphics::CloseCommandList(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList)
{
	HRESULT HR;

	HR = commandList->Close();
	ExitOnError(HR, "Failed to close the command list.");
}

void Graphics::PresentFrame(Microsoft::WRL::ComPtr<IDXGISwapChain3> swapChain, unsigned int* currentFrame)
{
	HRESULT HR;

	HR = swapChain->Present(1, 0);
	ExitOnError(HR, "Failed to present frame.");

	*currentFrame = swapChain->GetCurrentBackBufferIndex();
}

void Graphics::InitilizeGraphicsEngineShaders(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int numRT, DXGI_FORMAT* renderTargetFormats, DXGI_FORMAT depthStencilFormat, unsigned int samples)
{
	// Initialize shader to render G-buffer on quad.
	this->quadShader = DisplayShader::GetInstance();
	this->quadShader->Initialize(device, numRT, renderTargetFormats, depthStencilFormat, samples);

	// Initialize the directional light shader.
	DirectionalLightShader* dirLightShader = DirectionalLightShader::GetInstance();
	dirLightShader->Initialize(device, numRT, renderTargetFormats, depthStencilFormat, samples);
}

void Graphics::InitializeDeferredRendering(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int width, unsigned int height, unsigned int multiSamples)
{
	this->gBufferHeap_RTV.Initialize(device, GBufferRenderTarget::Size_RT, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);	// Remove one count for depth.
	this->gBufferHeap_DSV.Initialize(device, 1, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
	this->gBufferHeap_SRV.Initialize(device, GBufferShaderResource::Size_SR, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);

	// Position
	this->fragmentPositionFramebuffer.Initialize(device, gBufferFormats[GBufferRenderTarget::FragmentPosition_RT], width, height, multiSamples, this->gBufferClearColor[GBufferRenderTarget::FragmentPosition_RT], L"Position");
	this->fragmentPositionFramebuffer.SetFramebufferToRTVHandle(device, this->gBufferHeap_RTV.GetCPUHandle(GBufferRenderTarget::FragmentPosition_RT));
	this->fragmentPositionFramebuffer.SetFramebufferToSRVHandle(device, this->gBufferHeap_SRV.GetCPUHandle(GBufferShaderResource::FragmentPosition_SR));

	// Normal
	this->normalFramebuffer.Initialize(device, gBufferFormats[GBufferRenderTarget::Normal_RT], width, height, multiSamples, this->gBufferClearColor[GBufferRenderTarget::Normal_RT], L"Normal");
	this->normalFramebuffer.SetFramebufferToRTVHandle(device, this->gBufferHeap_RTV.GetCPUHandle(GBufferRenderTarget::Normal_RT));
	this->normalFramebuffer.SetFramebufferToSRVHandle(device, this->gBufferHeap_SRV.GetCPUHandle(GBufferShaderResource::Normal_SR));

	// Albedo
	this->albedoSpecFramebuffer.Initialize(device, this->gBufferFormats[GBufferRenderTarget::AlbedoSpecular_RT], width, height, multiSamples,this->gBufferClearColor[GBufferRenderTarget::AlbedoSpecular_RT], L"Albedo");
	this->albedoSpecFramebuffer.SetFramebufferToRTVHandle(device, this->gBufferHeap_RTV.GetCPUHandle(GBufferRenderTarget::AlbedoSpecular_RT));
	this->albedoSpecFramebuffer.SetFramebufferToSRVHandle(device, this->gBufferHeap_SRV.GetCPUHandle(GBufferShaderResource::AlbedoSpecular_SR));

	// Depth
	this->depthFramebuffer.Initialize(device, width, height, multiSamples, L"");
	this->depthFramebuffer.SetDepthBufferToDSVHandle(device, this->gBufferHeap_DSV.GetCPUHandle(0));
	this->depthFramebuffer.SetDepthBufferToSRVHandle(device, this->gBufferHeap_SRV.GetCPUHandle(GBufferShaderResource::Depth_SR));
}

void Graphics::SetDeferredFramebuffers(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList)
{
	// Position
	commandList->ClearRenderTargetView(this->gBufferHeap_RTV.GetCPUHandle(GBufferRenderTarget::FragmentPosition_RT), this->gBufferClearColor[GBufferRenderTarget::FragmentPosition_RT], 0, nullptr);
	// Normal
	commandList->ClearRenderTargetView(this->gBufferHeap_RTV.GetCPUHandle(GBufferRenderTarget::Normal_RT), this->gBufferClearColor[GBufferRenderTarget::Normal_RT], 0, nullptr);
	// Albedo
	commandList->ClearRenderTargetView(this->gBufferHeap_RTV.GetCPUHandle(GBufferRenderTarget::AlbedoSpecular_RT), this->gBufferClearColor[GBufferRenderTarget::AlbedoSpecular_RT], 0, nullptr);
	// Depth
	commandList->ClearDepthStencilView(this->gBufferHeap_DSV.GetCPUHandle(0), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	commandList->OMSetRenderTargets(GBufferRenderTarget::Size_RT, &this->gBufferHeap_RTV.GetCPUHandle(0), true, &this->gBufferHeap_DSV.GetCPUHandle(0));
}

void Graphics::RemoveDeferredFramebuffers(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList, unsigned int numRT, D3D12_CPU_DESCRIPTOR_HANDLE* newRTHandle, D3D12_CPU_DESCRIPTOR_HANDLE* newDepthHandle)
{
	commandList->OMSetRenderTargets(numRT, newRTHandle, (newDepthHandle == nullptr) ? false : true, newDepthHandle);

	this->fragmentPositionFramebuffer.CopyResource(commandList);
	this->normalFramebuffer.CopyResource(commandList);
	this->albedoSpecFramebuffer.CopyResource(commandList);
	this->depthFramebuffer.CopyResource(commandList);
}

void Graphics::InitializeQuadGeometry(Microsoft::WRL::ComPtr<ID3D12Device5> device)
{
	QuadVertex quadVertex[] =
	{
		QuadVertex(-1.0f, 1.0f, 0.0f, 0.0f, 1.0f),
		QuadVertex(-1.0f,-1.0f, 0.0f, 0.0f, 0.0f),
		QuadVertex( 1.0f, 1.0f, 0.0f, 1.0f, 1.0f),
		QuadVertex( 1.0f,-1.0f, 0.0f, 1.0f, 0.0f),
	};
	unsigned int quadBufferSize = sizeof(quadVertex);

	// Create the vertex buffer resource for quad.
	InitializeResource(&this->quadVertexBuffer, device, &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), &CD3DX12_RESOURCE_DESC::Buffer(quadBufferSize), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr);
	this->quadVertexBuffer->SetName(L"Quad Vertex Upload Buffer");

	// Fill the quad vertex data.
	unsigned char* pQuadVertexDataBegin;
	CD3DX12_RANGE readRange(0, 0);
	this->quadVertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pQuadVertexDataBegin));
	memcpy(pQuadVertexDataBegin, quadVertex, quadBufferSize);
	this->quadVertexBuffer->Unmap(0, nullptr);

	// Initialize vertex buffer view.
	this->quadVBV.BufferLocation = this->quadVertexBuffer->GetGPUVirtualAddress();
	this->quadVBV.SizeInBytes = quadBufferSize;
	this->quadVBV.StrideInBytes = sizeof(QuadVertex);
}

void Graphics::DrawQuadGeometry(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList)
{
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	commandList->IASetVertexBuffers(0, 1, &this->quadVBV);
	commandList->DrawInstanced(4, 1, 0, 0);
}

void Graphics::Initialize(HWND windowHandle, unsigned int width, unsigned int height)
{
	// Assign window handle.
	this->hWnd = windowHandle;

	// Assign viewport.
	this->viewport.TopLeftX = 0.0f;
	this->viewport.TopLeftY = 0.0f;
	this->viewport.Width = (float)width;
	this->viewport.Height = (float)height;

	this->viewport.MinDepth = 0.0f;
	this->viewport.MaxDepth = 1.0f;

	// Specify clipping rectangle.
	this->clippingRect.left = 0;
	this->clippingRect.top = 0;
	this->clippingRect.right = width;
	this->clippingRect.bottom = height;

	// Create device.
	this->CreateDevice();
	// Create command queue.
	this->CreateCommandQueue(&this->commandQueue, this->device, this->directCommandListType);
	// Create fence.
	this->CreateFence(&this->fence, &this->queueFenceValue, this->device);
	// Create swap chain.
	this->CreateSwapChain(&this->swapChain, this->device, this->factory, this->commandQueue, windowHandle, width, height, this->numberOfFrames, this->renderTargetFormat);
	// Create render target.
	this->CreateRenderTargets(&this->renderTargetHeap, this->renderTargetResource, this->frameCount, this->swapChain, this->device);
	// Create depth stencil.
	this->CreateDepthStencil(&this->depthStencilHeap, &this->depthStencilResource, this->depthStencilFormat, this->device, width, height);
	// Create command allocator.
	this->CreateCommandListSet(&this->commandAllocator, &this->commandList, &this->commandFenceValue, this->device, this->directCommandListType);

	// Multi samples count.
	unsigned int currentMultiSamples = (this->multiSampling) ? this->sampleCount : 1;

	// Create framebuffer.
	this->InitializeDeferredRendering(this->device, width, height, currentMultiSamples);

	// Initialize all graphics engine shaders.
	this->InitilizeGraphicsEngineShaders(device, 1, &this->renderTargetFormat, this->depthStencilFormat, 1);

	GameAssetManager* GAM = GameAssetManager::GetInstance();

	// Initialize the device to Game Asset Manager.
	GAM->SetGraphicsDevice(this->device);

	// Initialize the quad geometry.
	this->InitializeQuadGeometry(this->device);

	// Initialize All Shaders.
	GAM->InitializeAllShadersForDeferredRender(device, _countof(gBufferFormats), gBufferFormats, this->depthFramebuffer.GetRenderTargetDepthFormat(), currentMultiSamples);

	// Initialize the skymap.
	this->skymap.Initialize(this->device, this->commandList, this->skymapFileLocation);

	// LIGHT TEST.
	
	this->sunLight.Initialize(this->device);
	this->sunLight.SetFramebufferToFragmentPositionHandle(this->device, this->fragmentPositionFramebuffer);
	this->sunLight.SetFramebufferToNormalHandle(this->device, this->normalFramebuffer);
	this->sunLight.SetFramebufferToAlbedoSpecular(this->device, this->albedoSpecFramebuffer);
	this->sunLight.SetAmbientColor(Vector3(0.1f, 0.1f, 0.4f));
	this->sunLight.SetDiffuseColor(Vector3(1.0f, 1.0f, 1.0f));
	this->sunLight.SetSpecularColor(Vector3(1.0f, 1.0f, 1.0f));

	this->sunLightTransform.RotateX(40.0f);

	// Close and execute the command list.
	HRESULT HR = this->commandList->Close();
	ExitOnError(HR, "Failed to close the graphics command list.");

	ID3D12CommandList* ppCommandLists[] = { this->commandList.Get() };
	this->commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	this->commandFenceValue = this->GetCurrentSetFence();
	this->SignalFence(this->commandQueue, this->fence, this->commandFenceValue);
}

void Graphics::InitializeRenderList(RenderList* renderComponentList)
{
	GameAssetManager* GAM = GameAssetManager::GetInstance();

	this->WaitForFenceValue(this->fence, this->commandFenceValue);

	this->Reset(this->commandList, this->commandAllocator);

	renderComponentList->InitializeComponents(this->device, this->commandList);

	GAM->LoadAllAssetsTo_GPU_RAM(this->device, this->commandList);
	GAM->SetAllTextureViewToRespectiveHeaps(this->device);

	this->CloseCommandList(this->commandList);

	ID3D12CommandList* ppCommandLists[] = { this->commandList.Get() };
	this->commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	this->commandFenceValue = this->GetCurrentSetFence();
	SignalFence(this->commandQueue, this->fence, this->commandFenceValue);

	GAM->RemoveAssetsFrom_CPU_RAM();
}

void Graphics::RenderScene(RenderList* renderComponentList)
{
	this->WaitForFenceValue(this->fence, this->commandFenceValue);

	this->PresentFrame(this->swapChain, &this->currentFrame);

	this->Reset(this->commandList, this->commandAllocator);

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle;
	rtvHandle = this->renderTargetHeap.GetCPUHandle(this->currentFrame);
	Microsoft::WRL::ComPtr<ID3D12Resource> currentRenderBuffer = this->renderTargetResource[this->currentFrame];
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle;
	dsvHandle = this->depthStencilHeap.GetCPUHandle(0);

	commandList->RSSetViewports(1, &this->viewport);
	commandList->RSSetScissorRects(1, &this->clippingRect);

	TransitionResourceState(currentRenderBuffer, this->commandList, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	//----------------X-----------------//
	// G-BUFFER FILL.
	this->SetDeferredFramebuffers(this->commandList);
	
	// Recalculate camera matrix.
	this->mainCamera->CalculateViewProjection();
	GameAssetManager::GetInstance()->SetDescriptorHeap(this->commandList);

	renderComponentList->DrawAllComponents(this->commandList, this->mainCamera->GetCamera());

	this->skymap.Draw(this->commandList, this->mainCamera->GetCamera());

	this->RemoveDeferredFramebuffers(this->commandList, 1, &rtvHandle, &dsvHandle);

	//----------------X-----------------//
	// SHADOW RENDERING.
	// Draw all the objects in the scene for shadow mapping.

	if (this->useShadowRender)
	{
		this->sunLight.RenderSceneToShadow(this->commandList, 1, &rtvHandle, true, &dsvHandle, renderComponentList, Vector3(0.0f, 5.0f, 0.0f), &this->viewport, &this->clippingRect);
	}

	//----------------X-----------------//
	// POST-PROCESS DEBUG.
	// Clear the buffer and set render targets.
	static bool flag;
	float refreshColor[4] = { 0.0f, 0.5f, 0.0f, 1.0f };
	this->commandList->ClearRenderTargetView(rtvHandle, refreshColor, 0, nullptr);

	this->commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	this->commandList->OMSetRenderTargets(1, &rtvHandle, true, &dsvHandle);

	// Debug framebuffer using display shader on a quad geometry.
	if (this->useDirectionalLight)
	{
		this->sunLight.SetLightForRendering(this->commandList, this->mainCamera->GetCamera());
	}
	if (this->debugFramebuffer)
	{
		this->quadShader->SetShaderForRender(commandList);

		ID3D12DescriptorHeap* ppHeaps[] = { this->gBufferHeap_SRV.Get() };
		this->commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

		this->commandList->SetGraphicsRootDescriptorTable(0, this->gBufferHeap_SRV.GetGPUHandle(GBufferShaderResource::AlbedoSpecular_SR));
	}

	this->DrawQuadGeometry(commandList);
	//----------------X-----------------//
	TransitionResourceState(currentRenderBuffer, this->commandList, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	this->CloseCommandList(this->commandList);

	ID3D12CommandList* ppCommandLists[] = { this->commandList.Get() };
	this->commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	this->commandFenceValue = this->GetCurrentSetFence();
	SignalFence(this->commandQueue, this->fence, this->commandFenceValue);
}