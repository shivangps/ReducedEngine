#include "d3dx12.h"
#include "..//..//Helper.h"
#include "Graphics.h"
#include "..//..//Assets/DisplayShader/DisplayShader.h"
#include "..//..//Assets/DirectionalLightShader/DirectionalLightShader.h"
#include <random>
#include <thread>

UINT64 Graphics::GetCurrentSetFence()
{
	return ++this->queueFenceValue;
}

void Graphics::CreateCommandListSet(Microsoft::WRL::ComPtr<ID3D12CommandAllocator>* commandAllocator, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4>* commandList, UINT64* fenceValue, Microsoft::WRL::ComPtr<ID3D12Device5> device, D3D12_COMMAND_LIST_TYPE commandListType)
{
	CreateCommandAllocator(commandAllocator, device, commandListType);
	CreateGraphicsCommandList(commandList, device, *commandAllocator, commandListType);
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

void Graphics::InitializeDeferredRendering(Microsoft::WRL::ComPtr<ID3D12Device5> device, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList, unsigned int width, unsigned int height, unsigned int multiSamples)
{
	this->gBufferHeap_RTV.Initialize(device, GBufferRenderTarget::Size_RT, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);	// Remove one count for depth.
	this->gBufferHeap_DSV.Initialize(device, 1, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
	this->gBufferHeap_SRV.Initialize(device, GBufferShaderResource::Size_SR, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);

	// Position
	this->fragmentPositionFramebuffer.Initialize(device, gBufferFormats[GBufferRenderTarget::FragmentPosition_RT], width, height, multiSamples, this->gBufferClearColor[GBufferRenderTarget::FragmentPosition_RT], L"Fragment Position");
	this->fragmentPositionFramebuffer.SetFramebufferToRTVHandle(device, this->gBufferHeap_RTV.GetCPUHandle(GBufferRenderTarget::FragmentPosition_RT));
	this->fragmentPositionFramebuffer.SetFramebufferToSRVHandle(device, this->gBufferHeap_SRV.GetCPUHandle(GBufferShaderResource::FragmentPosition_SR));
	this->fragmentPositionHandle = this->universalDescriptorHeap->GetCbvSrvUavGPUHandle(this->universalDescriptorHeap->SetCpuHandle(
		this->device, this->fragmentPositionFramebuffer.GetResourceTexture(), this->fragmentPositionFramebuffer.GetShaderResourceView()));

	// View Position
	this->fragmentViewPositionFramebuffer.Initialize(device, gBufferFormats[GBufferRenderTarget::FragmentViewPosition_RT], width, height, multiSamples, this->gBufferClearColor[GBufferRenderTarget::FragmentViewPosition_RT], L"Fragment View Position");
	this->fragmentViewPositionFramebuffer.SetFramebufferToRTVHandle(device, this->gBufferHeap_RTV.GetCPUHandle(GBufferRenderTarget::FragmentViewPosition_RT));
	this->fragmentViewPositionFramebuffer.SetFramebufferToSRVHandle(device, this->gBufferHeap_SRV.GetCPUHandle(GBufferShaderResource::FragmentViewPosition_SR));
	this->fragmentViewPositionHandle = this->universalDescriptorHeap->GetCbvSrvUavGPUHandle(this->universalDescriptorHeap->SetCpuHandle(
		this->device, this->fragmentViewPositionFramebuffer.GetResourceTexture(), this->fragmentViewPositionFramebuffer.GetShaderResourceView()));

	// Normal
	this->normalFramebuffer.Initialize(device, gBufferFormats[GBufferRenderTarget::Normal_RT], width, height, multiSamples, this->gBufferClearColor[GBufferRenderTarget::Normal_RT], L"Normal");
	this->normalFramebuffer.SetFramebufferToRTVHandle(device, this->gBufferHeap_RTV.GetCPUHandle(GBufferRenderTarget::Normal_RT));
	this->normalFramebuffer.SetFramebufferToSRVHandle(device, this->gBufferHeap_SRV.GetCPUHandle(GBufferShaderResource::Normal_SR));
	this->normalPositionHandle = this->universalDescriptorHeap->GetCbvSrvUavGPUHandle(this->universalDescriptorHeap->SetCpuHandle(
		this->device, this->normalFramebuffer.GetResourceTexture(), this->normalFramebuffer.GetShaderResourceView()));

	// Albedo
	this->albedoSpecFramebuffer.Initialize(device, this->gBufferFormats[GBufferRenderTarget::AlbedoSpecular_RT], width, height, multiSamples,this->gBufferClearColor[GBufferRenderTarget::AlbedoSpecular_RT], L"Albedo");
	this->albedoSpecFramebuffer.SetFramebufferToRTVHandle(device, this->gBufferHeap_RTV.GetCPUHandle(GBufferRenderTarget::AlbedoSpecular_RT));
	this->albedoSpecFramebuffer.SetFramebufferToSRVHandle(device, this->gBufferHeap_SRV.GetCPUHandle(GBufferShaderResource::AlbedoSpecular_SR));
	this->albedoSpecularHandle = this->universalDescriptorHeap->GetCbvSrvUavGPUHandle(this->universalDescriptorHeap->SetCpuHandle(
		this->device, this->albedoSpecFramebuffer.GetResourceTexture(), this->albedoSpecFramebuffer.GetShaderResourceView()));

	// Depth
	this->depthFramebuffer.Initialize(device, width, height, multiSamples, L"");
	this->depthFramebuffer.SetDepthBufferToDSVHandle(device, this->gBufferHeap_DSV.GetCPUHandle(0));
	this->depthFramebuffer.SetDepthBufferToSRVHandle(device, this->gBufferHeap_SRV.GetCPUHandle(GBufferShaderResource::Depth_SR));
	this->depthHandle = this->universalDescriptorHeap->GetCbvSrvUavGPUHandle(this->universalDescriptorHeap->SetCpuHandle(
		this->device, this->depthFramebuffer.GetResourceTexture(), this->depthFramebuffer.GetShaderResourceView()));

	// SSAO
	this->IntializeSSAO(device, commandList, width, height);
}

void Graphics::ClearDeferredFramebuffers(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList)
{
	// Position
	commandList->ClearRenderTargetView(this->gBufferHeap_RTV.GetCPUHandle(GBufferRenderTarget::FragmentPosition_RT), this->gBufferClearColor[GBufferRenderTarget::FragmentPosition_RT], 0, nullptr);
	// View Position
	commandList->ClearRenderTargetView(this->gBufferHeap_RTV.GetCPUHandle(GBufferRenderTarget::FragmentViewPosition_RT), this->gBufferClearColor[GBufferRenderTarget::FragmentViewPosition_RT], 0, nullptr);
	// Normal
	commandList->ClearRenderTargetView(this->gBufferHeap_RTV.GetCPUHandle(GBufferRenderTarget::Normal_RT), this->gBufferClearColor[GBufferRenderTarget::Normal_RT], 0, nullptr);
	// Albedo
	commandList->ClearRenderTargetView(this->gBufferHeap_RTV.GetCPUHandle(GBufferRenderTarget::AlbedoSpecular_RT), this->gBufferClearColor[GBufferRenderTarget::AlbedoSpecular_RT], 0, nullptr);
	// Depth
	commandList->ClearDepthStencilView(this->gBufferHeap_DSV.GetCPUHandle(0), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
}

void Graphics::SetDeferredFramebuffers(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList)
{
	commandList->OMSetRenderTargets(GBufferRenderTarget::Size_RT, &this->gBufferHeap_RTV.GetCPUHandle(0), true, &this->gBufferHeap_DSV.GetCPUHandle(0));
}

void Graphics::RemoveDeferredFramebuffers(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList, unsigned int numRT, D3D12_CPU_DESCRIPTOR_HANDLE* newRTHandle, D3D12_CPU_DESCRIPTOR_HANDLE* newDepthHandle)
{
	commandList->OMSetRenderTargets(numRT, newRTHandle, (newDepthHandle == nullptr) ? false : true, newDepthHandle);

	this->fragmentPositionFramebuffer.CopyResource(commandList);
	this->fragmentViewPositionFramebuffer.CopyResource(commandList);
	this->normalFramebuffer.CopyResource(commandList);
	this->albedoSpecFramebuffer.CopyResource(commandList);
	this->depthFramebuffer.CopyResource(commandList);
}

void Graphics::InitalizeCommandLists(Microsoft::WRL::ComPtr<ID3D12Device5> device)
{
	this->preRenderCommandList.CreateSet(device);
	this->preRenderCommandList.commandList->SetName(L"Pre Render CommandList");
	this->preRenderCommandList.CloseCommandList();

	this->renderCommandList.CreateSet(device);
	this->renderCommandList.commandList->SetName(L"Render CommandList");
	this->renderCommandList.CloseCommandList();

	this->postRenderingCommandList.CreateSet(device);
	this->postRenderingCommandList.commandList->SetName(L"Post Render CommandList");
	this->postRenderingCommandList.CloseCommandList();
}

void Graphics::ExecuteCommandListsImmediately(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList, Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator)
{
	CloseCommandList(commandList);
	ID3D12CommandList* ppCommandList[] = { commandList.Get() };
	this->commandQueue->ExecuteCommandLists(_countof(ppCommandList), ppCommandList);
}

void Graphics::ExecuteCommandListsImmediately(CommandSet commandSet)
{
	commandSet.CloseCommandList();
	ID3D12CommandList* ppCommandList[] = { commandSet.commandList.Get() };
	this->commandQueue->ExecuteCommandLists(_countof(ppCommandList), ppCommandList);
}

void Graphics::IntializeSSAO(Microsoft::WRL::ComPtr<ID3D12Device5> device, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList, unsigned int width, unsigned int height)
{
	this->ambientOcclusion->Initialize(device, commandList, width, height);
	this->ambientOcclusion->SetFragmentViewPositionFramebufferToHeap(device, &this->fragmentViewPositionFramebuffer);
	this->ambientOcclusion->ApplyOutputTextureHandle(device, this->gBufferHeap_SRV.GetCPUHandle(GBufferShaderResource::SSAO_SR));	// For debugging.
	this->ambientOcclusion->SetNormalFramebufferToHeap(device, &this->normalFramebuffer);
	this->screenSpaceAmbientOcclusionHandle = *this->ambientOcclusion->GetGPUHandleForOutput();
}

void Graphics::InitializeMultipleCommandLists(Microsoft::WRL::ComPtr<ID3D12Device5> device)
{
	this->number_of_cpu_cores = std::thread::hardware_concurrency();

	// Use single thread if not able to detect the number of cores.
	if (this->number_of_cpu_cores <= 1)
		this->number_of_cpu_cores = 1;

	unsigned int numberOfCommandLists = 1;

	// Clip the number of threads spawning based on the maximum number specified.
	if (this->maximumThreadsToSpawn == 0 || this->maximumThreadsToSpawn >= this->number_of_cpu_cores)
		numberOfCommandLists = number_of_cpu_cores;
	else if (this->maximumThreadsToSpawn < this->number_of_cpu_cores)
		numberOfCommandLists = this->maximumThreadsToSpawn;

	// Initalize command lists and allocators based on the number of logical cores or SPU threads available.
	this->threadCommandSet.reserve(numberOfCommandLists);
	for (unsigned int i = 0; i < numberOfCommandLists; i++)
	{
		this->threadCommandSet.push_back(CommandSet());
		this->threadCommandSet[i].CreateSet(device);
		this->threadCommandSet[i].CloseCommandList();

		this->threadedCommandLists.push_back(this->threadCommandSet[i].commandList.Get());
	}
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
	// Create command allocator.
	this->CreateCommandListSet(&this->commandAllocator, &this->commandList, &this->commandFenceValue, this->device, this->directCommandListType);

	// Multi samples count.
	unsigned int currentMultiSamples = (this->multiSampling) ? this->sampleCount : 1;

	// Initialize the universal descriptor heap.
	this->universalDescriptorHeap->Initialize(this->device);

	// Create framebuffer.
	this->InitializeDeferredRendering(this->device, this->commandList, width, height, currentMultiSamples);

	// Initialize all graphics engine shaders.
	this->InitilizeGraphicsEngineShaders(device, 1, &this->renderTargetFormat, this->depthStencilFormat, 1);

	GameAssetManager* GAM = GameAssetManager::GetInstance();

	// Initialize the device to Game Asset Manager.
	GAM->SetGraphicsDevice(this->device);

	// Initialize the quad geometry.
	this->quad = Quad::GetInstance();
	this->quad->Initialize(device);

	// Initialize All Shaders.
	GAM->InitializeAllShadersForDeferredRender(device, _countof(gBufferFormats), gBufferFormats, this->depthFramebuffer.GetRenderTargetDepthFormat(), currentMultiSamples);

	// Initialize the skymap.
	this->skymap.Initialize(this->device, this->commandList, this->skymapFileLocation);

	// LIGHT TEST.
	this->sunLight.Initialize(this->device, width, height);

	this->sunLight.SetGpuHandleForFragmentPosition(this->fragmentPositionHandle);
	this->sunLight.SetGpuHandleForNormals(this->normalPositionHandle);
	this->sunLight.SetGpuHandleForAlbedoSpecular(this->albedoSpecularHandle);
	this->sunLight.SetGpuHandleForAmbientOcclusion(this->screenSpaceAmbientOcclusionHandle);
	this->sunLight.SetAmbientColor(Vector3(0.1f, 0.1f, 0.4f));
	this->sunLight.SetDiffuseColor(Vector3(1.0f, 1.0f, 1.0f));
	this->sunLight.SetSpecularColor(Vector3(1.0f, 1.0f, 1.0f));

	this->sunLightTransform.RotateX(40.0f);

	// Initialize the command lists for different rendering pipeline phases.
	this->InitalizeCommandLists(this->device);

	// Initialize multiple thread based command lists.
	this->InitializeMultipleCommandLists(this->device);

	// Close and execute the command list.
	HRESULT HR = this->commandList->Close();
	ExitOnError(HR, "Failed to close the graphics command list.");

	ID3D12CommandList* ppCommandLists[] = { this->commandList.Get() };
	this->commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	this->commandFenceValue = this->GetCurrentSetFence();
	SignalFence(this->commandQueue, this->fence, this->commandFenceValue);
}

void Graphics::InitializeRenderList(RenderList* renderComponentList)
{
	GameAssetManager* GAM = GameAssetManager::GetInstance();

	WaitForFenceValue(this->fence, this->commandFenceValue);

	Reset(this->commandList, this->commandAllocator);

	renderComponentList->InitializeComponents(this->device, this->commandList);

	GAM->LoadAllAssetsTo_GPU_RAM(this->device, this->commandList);
	GAM->SetAllTextureViewToRespectiveHeaps(this->device);

	CloseCommandList(this->commandList);

	ID3D12CommandList* ppCommandLists[] = { this->commandList.Get() };
	this->commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	this->commandFenceValue = this->GetCurrentSetFence();
	SignalFence(this->commandQueue, this->fence, this->commandFenceValue);

	GAM->RemoveAssetsFrom_CPU_RAM();

	renderComponentList->InitializeAllBundleLists();
}

void Graphics::RenderScene(RenderList* renderComponentList)
{
	WaitForFenceValue(this->fence, this->commandFenceValue);

	PresentFrame(this->swapChain, &this->currentFrame);

	this->preRenderCommandList.ResetAll();

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle;
	rtvHandle = this->renderTargetHeap.GetCPUHandle(this->currentFrame);
	Microsoft::WRL::ComPtr<ID3D12Resource> currentRenderBuffer = this->renderTargetResource[this->currentFrame];
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle;
	dsvHandle = this->depthStencilHeap.GetCPUHandle(0);

	this->timeDebugger.MarkTime();

	TransitionResourceState(currentRenderBuffer, this->preRenderCommandList.commandList, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	this->ClearDeferredFramebuffers(this->preRenderCommandList.commandList);

	// Set the background render color.
	Vector3 backgroundColor;
	backgroundColor = renderComponentList->GetBackgroundColor();
	this->gBufferClearColor[GBufferRenderTarget::AlbedoSpecular_RT][0] = backgroundColor.X();
	this->gBufferClearColor[GBufferRenderTarget::AlbedoSpecular_RT][1] = backgroundColor.Y();
	this->gBufferClearColor[GBufferRenderTarget::AlbedoSpecular_RT][2] = backgroundColor.Z();

	ExecuteCommandListsImmediately(this->preRenderCommandList.commandList, this->preRenderCommandList.commandAllocator);
	//----------------X-----------------//
	// G-BUFFER FILL.

	if (this->enableMultiThreadedRendering)
	{
		for (unsigned int i = 0; i < this->threadCommandSet.size(); i++)
		{
			this->threadCommandSet[i].ResetAll();

			this->threadCommandSet[i].commandList->RSSetViewports(1, &this->viewport);
			this->threadCommandSet[i].commandList->RSSetScissorRects(1, &this->clippingRect);

			this->SetDeferredFramebuffers(this->threadCommandSet[i].commandList);

			this->universalDescriptorHeap->AssignToGraphicsCommandList(this->threadCommandSet[i].commandList);
		}

		// Recalculate camera matrix.
		this->mainCamera->CalculateViewProjection();

		renderComponentList->DrawAllComponents(this->threadedCommandLists, this->mainCamera->GetCamera());

		this->skymap.Draw(this->threadCommandSet[0].commandList, this->mainCamera->GetCamera());

		for (unsigned int i = 0; i < this->threadCommandSet.size(); i++)
		{
			this->ExecuteCommandListsImmediately(this->threadCommandSet[i]);
		}
	}
	else
	{
		this->renderCommandList.ResetAll();

		this->renderCommandList.commandList->RSSetViewports(1, &this->viewport);
		this->renderCommandList.commandList->RSSetScissorRects(1, &this->clippingRect);

		this->SetDeferredFramebuffers(this->renderCommandList.commandList);

		this->universalDescriptorHeap->AssignToGraphicsCommandList(this->renderCommandList.commandList);

		// Recalculate camera matrix.
		this->mainCamera->CalculateViewProjection();

		renderComponentList->DrawAllComponents(this->renderCommandList.commandList, this->mainCamera->GetCamera());

		this->skymap.Draw(this->renderCommandList.commandList, this->mainCamera->GetCamera());

		this->ExecuteCommandListsImmediately(this->renderCommandList);
	}

	// END OF G-BUFFER FILL.
	//----------------X-----------------//

	// Transition framebuffer resources.
	this->postRenderingCommandList.ResetAll();

	this->postRenderingCommandList.commandList->RSSetViewports(1, &this->viewport);
	this->postRenderingCommandList.commandList->RSSetScissorRects(1, &this->clippingRect);

	this->RemoveDeferredFramebuffers(this->postRenderingCommandList.commandList, 1, &rtvHandle, &dsvHandle);

	this->universalDescriptorHeap->AssignToGraphicsCommandList(this->postRenderingCommandList.commandList);

	//----------------X-----------------//
	// SSAO
	this->ambientOcclusion->PostProcessAmbientOcclusion(this->postRenderingCommandList.commandList, this->mainCamera->GetCamera());

	//----------------X-----------------//
	// SHADOW RENDERING.
	// Draw all the objects in the scene for shadow mapping.

	this->sunLight.RenderSceneToShadowDepth(this->postRenderingCommandList.commandList, 1, &rtvHandle, true, &dsvHandle, renderComponentList, Vector3(1.0f, 5.0f, 0.0f), &this->viewport, &this->clippingRect);

	//----------------X-----------------//
	// Shadow Rendering
	this->sunLight.RenderShadow(this->postRenderingCommandList.commandList);

	//----------------X-----------------//
	// POST-PROCESS DEBUG.
	// Clear the buffer and set render targets.
	float refreshColor[4] = { 0.0f, 0.5f, 0.0f, 1.0f };
	this->postRenderingCommandList.commandList->ClearRenderTargetView(rtvHandle, refreshColor, 0, nullptr);

	this->postRenderingCommandList.commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	this->postRenderingCommandList.commandList->OMSetRenderTargets(1, &rtvHandle, true, &dsvHandle);

	// Debug framebuffer using display shader on a quad geometry.
	if (!this->debugFramebuffer)
	{
		this->sunLight.SetLightForRendering(this->postRenderingCommandList.commandList, this->mainCamera->GetCamera());
	}
	if (this->debugFramebuffer)
	{
		this->postRenderingCommandList.commandList->SetGraphicsRootDescriptorTable(0, fragmentPositionHandle);

		this->quadShader->SetShaderForRender(this->postRenderingCommandList.commandList);

		this->quad->Draw(this->postRenderingCommandList.commandList);
	}
	//----------------X-----------------//
	TransitionResourceState(currentRenderBuffer, this->postRenderingCommandList.commandList, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

	CloseCommandList(this->postRenderingCommandList.commandList);

	ID3D12CommandList* ppCommandLists[] = { this->postRenderingCommandList.commandList.Get() };
	this->commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	this->commandFenceValue = this->GetCurrentSetFence();
	SignalFence(this->commandQueue, this->fence, this->commandFenceValue);

	this->timeDebugger.MarkAndDisplayTime("Overall Draw Recording Duration");
}