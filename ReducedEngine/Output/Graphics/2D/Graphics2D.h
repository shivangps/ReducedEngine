#pragma once
#include "../GraphicsHelper.h"
#include "../RenderList.h"
#include "../UniversalDescriptorHeap.h"
#include "../Quad/Quad.h"
#include "../RenderFramebuffer.h"
#include "../DepthFramebuffer.h"
#include "../Assets/2D_Assets/Shaders/PostDisplayShader/PostDisplayShader.h"

// Class that handles the rendering of 2D objects present in the scene.

class Graphics2D
{
private:
	Graphics2D() {}
	~Graphics2D()
	{
		this->DestroyAndReleaseAll();
	}

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

	UINT64 commandFenceValue = 0;
	CommandSet mainCommandSet = {};

	float backgroundColor[4] = {0.0f, 0.5f, 0.0f, 0.0f};

	// Function to get the currently set fence value for command queue.
	UINT64 GetCurrentSetFence();
	// Function to initialize all the graphics engine shaders.
	void InitilizeGraphicsEngineShaders(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int numRT, DXGI_FORMAT* renderTargetFormats, DXGI_FORMAT depthStencilFormat, unsigned int samples);
	// Function to execute the command lists in queue.
	void ExecuteCommandLists(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList);
	// Function to destroy all the resources and heaps and release it from memory if any.
	void DestroyAndReleaseAll();

private:
	// Boolean to apply anti aliasing using mutli sampling.
	bool multiSampling = true;
	unsigned int sampleCount = 4;

	// Descriptor heap used by all the render components.
	UniversalDescriptorHeap* universalDescriptorHeap = UniversalDescriptorHeap::GetInstance();

	// Quad for rendering post-processing effects.
	Quad* quad = Quad::GetInstance();

	// Game asset manager.
	GameAssetManager* GAM = GameAssetManager::GetInstance();

	// Main camera holder.
	MainCamera2D* mainCamera2d = MainCamera2D::GetInstance();

private:
	// Deffered rendering.
	RenderFramebuffer albedoFramebuffer = {};
	DepthFramebuffer depthFramebuffer = {};

	// G-Buffer render targets.
	enum G_Buffer_RT
	{
		Albedo_RT = 0,
		Size_RT,
	};

	DescriptorHeap gBuffer_RenderHeap = {};
	DescriptorHeap gBuffer_DepthHeap = {};

	DXGI_FORMAT gBufferFormats[G_Buffer_RT::Size_RT] = {
		DXGI_FORMAT_R32G32B32A32_FLOAT,				// Albedo
	};

	float albedoBackgroundColor[4] = {0.05f, 0.05f, 0.05f, 0.0f};

	// G-Buffer shader resource.
	enum G_Buffer_SR
	{
		Albedo_SR = 0,
		Size_SR,
	};

	D3D12_GPU_DESCRIPTOR_HANDLE albedoGpuHandle = {};

	// Function to initalize the deffered rendering process.
	void InitializeDefferedRendering(Microsoft::WRL::ComPtr<ID3D12Device5> device, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList, unsigned int width, unsigned int height, unsigned int multiSamples);
	// Function to clear the deffered framebuffers.
	void ClearGBuffers(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList);
	// Function to set the framebuffers for rendering the scene.
	void SetGBuffers(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList);
	// Function to remove the framebuffers from rendering.
	void RemoveGBuffers(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList, unsigned int numRT, D3D12_CPU_DESCRIPTOR_HANDLE* newRTHandle, D3D12_CPU_DESCRIPTOR_HANDLE* newDepthHandle);

	// Shader to display the framebuffer.
	Shader* postDisplayShader = PostDisplayShader::GetInstance();

public:
	// Function to get the a single instance of the 2d graphics engine.
	static Graphics2D* GetInstance()
	{
		static Graphics2D* instance = new Graphics2D();
		return instance;
	}

	// Function to call for initializing the render engine and directx.
	void Initialize(HWND windowHandle, unsigned int width, unsigned int height);
	// Function to initialize render componenet list.
	void InitializeRenderList(RenderList* renderComponentList);
	// Function to call for rendering the whole scene.
	void RenderScene(RenderList* renderComponentList);
};