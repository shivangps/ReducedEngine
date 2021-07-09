#pragma once
#include "GraphicsHelper.h"

// Header file that contains class to initialize and maintain depth framebuffer.

class DepthFramebuffer
{
private:
	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilBuffer = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> textureBuffer = nullptr;

	DXGI_FORMAT depthRtFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	DXGI_FORMAT depthSrFormat = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;

	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};

	unsigned int multiSamples = 1;

public:
	// Function to intialize the depth framebuffer.
	void Initialize(Microsoft::WRL::ComPtr<ID3D12Device5> device,
		unsigned int width, unsigned int height, unsigned int multiSamples, std::wstring depthBufferName);
	// Function to get the depth stencil format of render target.
	DXGI_FORMAT GetRenderTargetDepthFormat();
	// Function to set depth buffer as a render target to cpu descriptor handle.
	void SetDepthBufferToDSVHandle(Microsoft::WRL::ComPtr<ID3D12Device5> device, D3D12_CPU_DESCRIPTOR_HANDLE handle);
	// Function to set depth buffer as a shader resource to cpu descriptor handle.
	void SetDepthBufferToSRVHandle(Microsoft::WRL::ComPtr<ID3D12Device5> device, D3D12_CPU_DESCRIPTOR_HANDLE handle);
	// Function to copy the image from multi sampled depth buffer resource to unsampled shader resource.
	void CopyResource(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList);
};