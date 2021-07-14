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
	// Function to initialize all the graphics engine shaders.
	void InitilizeGraphicsEngineShaders(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int numRT, DXGI_FORMAT* renderTargetFormats, DXGI_FORMAT depthStencilFormat, unsigned int samples);

private:
	// Deffered rendering.
	// Framebuffer render target.
	enum GBufferRenderTarget
	{
		FragmentPosition_RT = 0,
		Normal_RT,
		AlbedoSpecular_RT,
		Size_RT
	};
	DescriptorHeap gBufferHeap_RTV = {};

	DXGI_FORMAT gBufferFormats[GBufferRenderTarget::Size_RT] = {
		DXGI_FORMAT_R16G16B16A16_FLOAT,				// Position
		DXGI_FORMAT_R16G16B16A16_FLOAT,				// Normal
		DXGI_FORMAT_R16G16B16A16_FLOAT,				// Albedo & Specular
	};
	float gBufferClearColor[GBufferRenderTarget::Size_RT][4] =
	{
		{ 0.0f, 0.0f, 0.0f, 0.0f },			// Position
		{ 0.0f, 0.0f, 0.0f, 0.0f },			// Normal
		{ 0.0f, 0.0f, 0.0f, 0.0f },			// Albedo & Specular
	};
	RenderFramebuffer fragmentPositionFramebuffer = {};
	RenderFramebuffer normalFramebuffer = {};
	RenderFramebuffer albedoSpecFramebuffer = {};
	
	DepthFramebuffer depthFramebuffer = {};

	DescriptorHeap gBufferHeap_DSV = {};

	// Framebuffer shader resource.
	enum GBufferShaderResource
	{
		FragmentPosition_SR = 0,
		Normal_SR,
		AlbedoSpecular_SR,
		Depth_SR,
		Size_SR
	};
	DescriptorHeap gBufferHeap_SRV = {};

	// Boolean to apply anti aliasing using mutli sampling.
	bool multiSampling = true;
	unsigned int sampleCount = 4;

	// Function to initialize the deferred rendering process.
	void InitializeDeferredRendering(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int width, unsigned int height, unsigned int multiSamples);
	// Function to set the framebuffers for rendering the scene.
	void SetDeferredFramebuffers(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList);
	// Function to remove the framebuffers from rendering.
	void RemoveDeferredFramebuffers(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList, unsigned int numRT, D3D12_CPU_DESCRIPTOR_HANDLE* newRTHandle, D3D12_CPU_DESCRIPTOR_HANDLE* newDepthHandle);

private:
	// Deferred render display.
	Shader* quadShader = nullptr;

	// The following definition is for rendering the quad and applying the texture of a post processed scene.
	// Vertex for quad.
	struct QuadVertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 texCoord;

		QuadVertex(float x,float y,float z,float u,float v) : position(x, y, z), texCoord(u, v) {}
	};
	Microsoft::WRL::ComPtr<ID3D12Resource> quadVertexBuffer = nullptr;
	D3D12_VERTEX_BUFFER_VIEW quadVBV = {};
	
	// Function to initialize the geometry of a quad covering the whole scene.
	void InitializeQuadGeometry(Microsoft::WRL::ComPtr<ID3D12Device5> device);
	// Function to call for drawing the whole scene.
	void DrawQuadGeometry(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList);

private:
	MainCamera* mainCamera = MainCamera::GetInstance();

private:
	Cubemap skymap;
	std::string skymapFileLocation = "Assets/Textures/Skymap/Skymap.dds";

private:
	// Sun Light as Directional Light.
	Transform sunLightTransform = {};
	LightComponent sunLight = { &this->sunLightTransform };

	bool useShadowRender = true;
	bool useDirectionalLight = true;
	bool debugFramebuffer = false;

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