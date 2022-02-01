#pragma once

// Graphics headers.
#include "../../CommonHeader.h"
#include "GraphicsHelper.h"
#include "RenderFramebuffer.h"
#include "DepthFramebuffer.h"
#include "Shader.h"
#include "RenderList.h"
#include <dxgi1_4.h>
#include "Camera.h"
#include "Cubemap.h"
#include "LightComponent.h"
#include "../Tools/TimeTool.h"
#include "Quad/Quad.h"
#include "SSAO/SSAO.h"

// Class that handles the rendering of objects present in the scene.

// This is a singleton.
class Graphics
{
	Graphics() {}

	HWND hWnd = NULL;

	D3D12_VIEWPORT viewport = {};
	D3D12_RECT clippingRect = {};

	Microsoft::WRL::ComPtr<ID3D12Device5> device = nullptr;

	GraphicsDeviceManager deviceManager = {};

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

	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator = nullptr;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList = nullptr;
	UINT64 commandFenceValue = 0;

	// Function to get queue fence value.
	UINT64 GetCurrentSetFence();

	// Function to create command list all together with command allocator.
	void CreateCommandListSet(Microsoft::WRL::ComPtr<ID3D12CommandAllocator>* commandAllocator, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4>* commandList, UINT64* fenceValue, Microsoft::WRL::ComPtr<ID3D12Device5> device, D3D12_COMMAND_LIST_TYPE commandListType);

	// Function to initialize all the graphics engine shaders.
	void InitilizeGraphicsEngineShaders(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int numRT, DXGI_FORMAT* renderTargetFormats, DXGI_FORMAT depthStencilFormat, unsigned int samples);

private:
	UniversalDescriptorHeap* universalDescriptorHeap = UniversalDescriptorHeap::GetInstance();

	// Deffered rendering.
	// Framebuffer render target.
	enum GBufferRenderTarget
	{
		FragmentPosition_RT = 0,
		FragmentViewPosition_RT,
		Normal_RT,
		AlbedoSpecular_RT,
		Size_RT
	};
	DescriptorHeap gBufferHeap_RTV = {};

	DXGI_FORMAT gBufferFormats[GBufferRenderTarget::Size_RT] = {
		DXGI_FORMAT_R16G16B16A16_FLOAT,				// Position
		DXGI_FORMAT_R16G16B16A16_FLOAT,				// View Position
		DXGI_FORMAT_R16G16B16A16_FLOAT,				// Normal
		DXGI_FORMAT_R16G16B16A16_FLOAT,				// Albedo & Specular
	};
	float gBufferClearColor[GBufferRenderTarget::Size_RT][4] =
	{
		{ 0.0f, 0.0f, 0.0f, 0.0f },			// Position
		{ 0.0f, 0.0f, 0.0f, 0.0f },			// View Position
		{ 0.0f, 0.0f, 0.0f, 0.0f },			// Normal
		{ 0.0f, 0.0f, 0.0f, 0.0f },			// Albedo & Specular
	};
	RenderFramebuffer fragmentPositionFramebuffer = {};
	RenderFramebuffer fragmentViewPositionFramebuffer = {};
	RenderFramebuffer normalFramebuffer = {};
	RenderFramebuffer albedoSpecFramebuffer = {};
	
	DepthFramebuffer depthFramebuffer = {};

	DescriptorHeap gBufferHeap_DSV = {};

	CommandSet preRenderCommandList = {};
	CommandSet renderCommandList = {};
	CommandSet postRenderingCommandList = {};

	// Framebuffer shader resource.
	enum GBufferShaderResource
	{
		FragmentPosition_SR = 0,
		FragmentViewPosition_SR,
		Normal_SR,
		AlbedoSpecular_SR,
		Depth_SR,
		SSAO_SR,
		Size_SR
	};
	DescriptorHeap gBufferHeap_SRV = {};

	D3D12_GPU_DESCRIPTOR_HANDLE fragmentPositionHandle = {};
	D3D12_GPU_DESCRIPTOR_HANDLE fragmentViewPositionHandle = {};
	D3D12_GPU_DESCRIPTOR_HANDLE normalPositionHandle = {};
	D3D12_GPU_DESCRIPTOR_HANDLE albedoSpecularHandle = {};
	D3D12_GPU_DESCRIPTOR_HANDLE depthHandle = {};
	D3D12_GPU_DESCRIPTOR_HANDLE screenSpaceAmbientOcclusionHandle = {};

	// Boolean to apply anti aliasing using mutli sampling.
	bool multiSampling = true;
	unsigned int sampleCount = 4;

	// Function to initialize the deferred rendering process.
	void InitializeDeferredRendering(Microsoft::WRL::ComPtr<ID3D12Device5> device, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList, unsigned int width, unsigned int height, unsigned int multiSamples);
	// Function to clear the framebuffers.
	void ClearDeferredFramebuffers(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList);
	// Function to set the framebuffers for rendering the scene.
	void SetDeferredFramebuffers(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList);
	// Function to remove the framebuffers from rendering.
	void RemoveDeferredFramebuffers(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList, unsigned int numRT, D3D12_CPU_DESCRIPTOR_HANDLE* newRTHandle, D3D12_CPU_DESCRIPTOR_HANDLE* newDepthHandle);
	// Function to initialize command lists for seperate rendering sections.
	void InitalizeCommandLists(Microsoft::WRL::ComPtr<ID3D12Device5> device);
	// Function to immediately start executing command list set.
	void ExecuteCommandListsImmediately(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList, Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator);
	void ExecuteCommandListsImmediately(CommandSet commandSet);

private:
	// Deferred render display.
	Shader* quadShader = nullptr;

	// Single instance class to store and render a quad geometry covering the whole screen for post-processing effects.
	Quad* quad = nullptr;

private:
	MainCamera* mainCamera = MainCamera::GetInstance();

private:
	Cubemap skymap;
	std::string skymapFileLocation = "Assets/Textures/Skymap/Skymap.dds";

private:
	// Sun Light as Directional Light.
	Transform sunLightTransform = {};
	LightComponent sunLight = { &this->sunLightTransform };

	bool debugFramebuffer = false;

private:
	// Applying time marking tool for debugging purposes.
	TimeTool timeDebugger;

private:
	// Ambient Occlusion.
	SSAO* ambientOcclusion = SSAO::GetInstance();

	void IntializeSSAO(Microsoft::WRL::ComPtr<ID3D12Device5> device, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList, unsigned int width, unsigned int height);

private:
	// Multi-threaded rendering.
	bool enableMultiThreadedRendering = false;
	unsigned int number_of_cpu_cores = 0;
	std::vector<CommandSet> threadCommandSet = {};
	std::vector<ID3D12GraphicsCommandList4*> threadedCommandLists = {};
	// Set as 0 for as many as the logical cores of cpu.
	unsigned int maximumThreadsToSpawn = 4;

	void InitializeMultipleCommandLists(Microsoft::WRL::ComPtr<ID3D12Device5> device);

public:
	// Function to get a single instance of graphics or render engine.
	static Graphics* GetInstance()
	{
		static Graphics* instance = new Graphics();
		return instance;
	}

	// Function to call for initializing the render engine and directx.
	void Initialize(HWND windowHandle, unsigned int width, unsigned int height);
	// Function to initialize render componenet list.
	void InitializeRenderList(RenderList* renderComponentList);
	// Function to call for rendering the whole scene.
	void RenderScene(RenderList* renderComponentList);
};