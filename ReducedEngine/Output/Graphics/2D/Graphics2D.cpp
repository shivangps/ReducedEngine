#include "Graphics2D.h"

UINT64 Graphics2D::GetCurrentSetFence()
{
	return ++this->queueFenceValue;
}

void Graphics2D::InitilizeGraphicsEngineShaders(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int numRT, DXGI_FORMAT* renderTargetFormats, DXGI_FORMAT depthStencilFormat, unsigned int samples)
{
	this->postDisplayShader->Initialize(device, numRT, renderTargetFormats, depthStencilFormat, samples);
}

void Graphics2D::ExecuteCommandLists(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList)
{
	ID3D12CommandList* ppCommandLists[] = { commandList.Get() };
	this->commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
}

void Graphics2D::DestroyAndReleaseAll()
{
	this->device->Release();
	this->GAM->ReleaseAllAssets();
	this->universalDescriptorHeap->Release();
	this->depthStencilHeap.Release();
	this->renderTargetHeap.Release();
	this->mainCommandSet.ReleaseAll();
	this->commandQueue->Release();
	this->swapChain->Release();
}

void Graphics2D::InitializeDefferedRendering(Microsoft::WRL::ComPtr<ID3D12Device5> device, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList, unsigned int width, unsigned int height, unsigned int multiSamples)
{
	this->gBuffer_RenderHeap.Initialize(device, G_Buffer_RT::Size_RT, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);	// Remove one count for depth.
	this->gBuffer_DepthHeap.Initialize(device, 1, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);

	// Albedo
	this->albedoFramebuffer.Initialize(device, this->gBufferFormats[G_Buffer_RT::Albedo_RT], width, height, multiSamples, this->albedoBackgroundColor, L"2D Albedo");
	this->albedoFramebuffer.SetFramebufferToRTVHandle(device, this->gBuffer_RenderHeap.GetCPUHandle(G_Buffer_RT::Albedo_RT));
	this->albedoGpuHandle = this->universalDescriptorHeap->GetCbvSrvUavGPUHandle(this->universalDescriptorHeap->SetCpuHandle(
		device, this->albedoFramebuffer.GetResourceTexture(), this->albedoFramebuffer.GetShaderResourceView()));

	// Depth
	this->depthFramebuffer.Initialize(device, width, height, multiSamples, L"2D");
	this->depthFramebuffer.SetDepthBufferToDSVHandle(device, this->gBuffer_DepthHeap.GetCPUHandle(0));
}

void Graphics2D::ClearGBuffers(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList)
{
	// Albedo
	commandList->ClearRenderTargetView(this->gBuffer_RenderHeap.GetCPUHandle(G_Buffer_RT::Albedo_RT), this->albedoBackgroundColor, 0, nullptr);
	// Depth
	commandList->ClearDepthStencilView(this->gBuffer_DepthHeap.GetCPUHandle(0), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
}

void Graphics2D::SetGBuffers(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList)
{
	commandList->OMSetRenderTargets(G_Buffer_RT::Size_RT, &this->gBuffer_RenderHeap.GetCPUHandle(0), true, &this->gBuffer_DepthHeap.GetCPUHandle(0));
}

void Graphics2D::RemoveGBuffers(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList, unsigned int numRT, D3D12_CPU_DESCRIPTOR_HANDLE* newRTHandle, D3D12_CPU_DESCRIPTOR_HANDLE* newDepthHandle)
{
	commandList->OMSetRenderTargets(numRT, newRTHandle, (newDepthHandle == nullptr) ? false : true, newDepthHandle);

	this->albedoFramebuffer.CopyResource(commandList);
	this->depthFramebuffer.CopyResource(commandList);
}

void Graphics2D::Initialize(HWND windowHandle, unsigned int width, unsigned int height)
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
	this->deviceManager.CreateDevice();
	this->device = this->deviceManager.GetDevice();
	// Create command queue.
	this->deviceManager.CreateCommandQueue(&this->commandQueue, this->directCommandListType);
	// Create fence.
	CreateFence(&this->fence, &this->queueFenceValue, this->device);
	// Create swap chain.
	this->deviceManager.CreateSwapChain(&this->swapChain, this->commandQueue, this->hWnd, width, height, this->numberOfFrames, this->renderTargetFormat);
	// Create render target.
	CreateRenderTargetsFromSwapChain(&this->renderTargetHeap, this->renderTargetResource, this->frameCount, this->swapChain, this->device);
	// Create depth stencil.
	CreateDepthStencil(&this->depthStencilHeap, &this->depthStencilResource, this->depthStencilFormat, this->device, width, height);
	// Create command list and command allocator.
	this->mainCommandSet.CreateSet(this->device, "Main");

	// Multi samples count.
	unsigned int currentMultiSamples = (this->multiSampling) ? this->sampleCount : 1;

	// Initialize the universal descriptor heap.
	this->universalDescriptorHeap->Initialize(this->device);

	GameAssetManager* gameAssetManager = GameAssetManager::GetInstance();

	// Initialize the device to Game Asset Manager.
	gameAssetManager->SetGraphicsDevice(this->device);

	// Initialize all graphics engine shaders.
	this->InitilizeGraphicsEngineShaders(device, 1, &this->renderTargetFormat, this->depthStencilFormat, 1);

	// Initialize the deffered rendering resources.
	this->InitializeDefferedRendering(this->device, this->mainCommandSet.commandList, width, height, currentMultiSamples);

	// Initialize the quad geometry.
	this->quad = Quad::GetInstance();
	this->quad->Initialize(this->device);

	// Shader initialization for 2D.
	this->GAM->InitializeAllShadersFor2DDeferredRender(this->device, G_Buffer_RT::Size_RT, this->gBufferFormats, this->depthStencilFormat, currentMultiSamples);

	// GUIEngine initialisation.
	this->guiEngine->Initalize(this->device, this->mainCommandSet.commandList, this->output->GetWindowWidth(), this->output->GetWindowHeight());
	this->uiGpuHandle = this->guiEngine->GetUISrvGpuHandle();

	// Close and execute the command list.
	this->mainCommandSet.CloseCommandList();
	this->ExecuteCommandLists(this->mainCommandSet.commandList);
	this->commandFenceValue = this->GetCurrentSetFence();
	SignalFence(this->commandQueue, this->fence, this->commandFenceValue);
}

void Graphics2D::InitializeRenderList(RenderList* renderComponentList)
{
	GameAssetManager* gameAssetManager = GameAssetManager::GetInstance();

	WaitForFenceValue(this->fence, this->commandFenceValue);

	this->mainCommandSet.ResetAll();

	renderComponentList->InitializeComponents2D(this->device);

	gameAssetManager->LoadAllAssetsTo_GPU_RAM(this->device, this->mainCommandSet.commandList);

	this->mainCommandSet.CloseCommandList();

	this->ExecuteCommandLists(this->mainCommandSet.commandList);

	this->commandFenceValue = this->GetCurrentSetFence();
	SignalFence(this->commandQueue, this->fence, this->commandFenceValue);

	gameAssetManager->RemoveAssetsFrom_CPU_RAM();
}

void Graphics2D::InitializeGUIComponentList(GUIComponentList* guiComponentList)
{
	WaitForFenceValue(this->fence, this->commandFenceValue);

	this->mainCommandSet.ResetAll();

	this->guiEngine->InitializeGUICompnentList(guiComponentList, device, mainCommandSet.commandList);

	this->mainCommandSet.CloseCommandList();

	this->ExecuteCommandLists(this->mainCommandSet.commandList);

	this->commandFenceValue = this->GetCurrentSetFence();
	SignalFence(this->commandQueue, this->fence, this->commandFenceValue);
}

void Graphics2D::RenderScene(RenderList* renderComponentList, GUIComponentList* guiComponentList)
{
	WaitForFenceValue(this->fence, this->commandFenceValue);

	PresentFrame(this->swapChain, &this->currentFrame);

	this->mainCommandSet.ResetCommandAllocator();
	this->mainCommandSet.ResetCommandList();

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle;
	rtvHandle = this->renderTargetHeap.GetCPUHandle(this->currentFrame);
	Microsoft::WRL::ComPtr<ID3D12Resource> currentRenderBuffer = this->renderTargetResource[this->currentFrame];
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle;
	dsvHandle = this->depthStencilHeap.GetCPUHandle(0);

	TransitionResourceState(currentRenderBuffer, this->mainCommandSet.commandList, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	this->universalDescriptorHeap->AssignToGraphicsCommandList(this->mainCommandSet.commandList);
	// MAIN RENDERING.

	this->mainCommandSet.commandList->RSSetViewports(1, &this->viewport);
	this->mainCommandSet.commandList->RSSetScissorRects(1, &this->clippingRect);

	// G-BUFFER FILL.
	// Setting background color from render list.
	Vector3 backgroundColor = renderComponentList->GetBackgroundColor();
	this->albedoBackgroundColor[0] = backgroundColor.X();
	this->albedoBackgroundColor[1] = backgroundColor.Y();
	this->albedoBackgroundColor[2] = backgroundColor.Z();

	this->ClearGBuffers(this->mainCommandSet.commandList);
	this->SetGBuffers(this->mainCommandSet.commandList);

	this->mainCamera2d->CalculateViewProjection();

	renderComponentList->DrawAll2DComponents(this->mainCommandSet.commandList, this->mainCamera2d->GetCamera2D());

	this->RemoveGBuffers(this->mainCommandSet.commandList, 1, &rtvHandle, &dsvHandle);

	// UI RENDERING.
	this->guiEngine->RenderGUI(guiComponentList, this->mainCommandSet.commandList, 1, &rtvHandle, &dsvHandle);

	// POST RENDERING.
	this->mainCommandSet.commandList->ClearRenderTargetView(rtvHandle, this->backgroundColor, 0, nullptr);
	this->mainCommandSet.commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	this->postDisplayShader->SetShaderForRender(this->mainCommandSet.commandList);

	this->mainCommandSet.commandList->SetGraphicsRootDescriptorTable(0, this->albedoGpuHandle);
	this->mainCommandSet.commandList->SetGraphicsRootDescriptorTable(1, this->uiGpuHandle);

	this->quad->Draw(this->mainCommandSet.commandList);

	// MAIN RENDERING END.
	TransitionResourceState(currentRenderBuffer, this->mainCommandSet.commandList, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	
	this->mainCommandSet.CloseCommandList();
	this->ExecuteCommandLists(this->mainCommandSet.commandList);
	this->commandFenceValue = this->GetCurrentSetFence();
	SignalFence(this->commandQueue, this->fence, this->commandFenceValue);
}
