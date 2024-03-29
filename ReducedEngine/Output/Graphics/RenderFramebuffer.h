#pragma once
#include "GraphicsHelper.h"
#include "UniversalDescriptorHeap.h"

// Header file that contains class to initialize and maintain a render framebuffer.

class RenderFramebuffer
{
private:
	Microsoft::WRL::ComPtr<ID3D12Resource> renderTargetBuffer = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> textureBuffer = nullptr;
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	DXGI_FORMAT framebufferFormat = {};
	unsigned int multisamples = 1;
	float clearColor[4];
	D3D12_GPU_DESCRIPTOR_HANDLE gpuSrvHandle = {};

public:
	// Function to initialize the render framebuffer.
	void Initialize(Microsoft::WRL::ComPtr<ID3D12Device5> device, DXGI_FORMAT format,
		unsigned int width, unsigned int height, unsigned int multiSamples, float clearColor[4], std::wstring framebufferName);
	// Function to set framebuffer as a render target to cpu descriptor handle.
	void SetFramebufferToRTVHandle(Microsoft::WRL::ComPtr<ID3D12Device5> device, D3D12_CPU_DESCRIPTOR_HANDLE handle);
	// Function to set framebuffer as a shader resource to cpu descriptor handle.
	void SetFramebufferToSRVHandle(Microsoft::WRL::ComPtr<ID3D12Device5> device, D3D12_CPU_DESCRIPTOR_HANDLE handle);
	// Function to copy the image from multi sampled render target resource to unsampled shader resource.
	void CopyResource(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList);
	// Function to get the shader resource view of the framebuffer.
	D3D12_SHADER_RESOURCE_VIEW_DESC* GetShaderResourceView();
	// Function to get the resource of the texture framebuffer.
	ID3D12Resource* GetResourceTexture();
	// Function to get the gpu descriptor handle in the universal descriptor heap.
	D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandleForShaderResource();
};