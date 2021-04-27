#include "RenderFramebuffer.h"

void RenderFramebuffer::Initialize(Microsoft::WRL::ComPtr<ID3D12Device5> device, DXGI_FORMAT format, unsigned int width, unsigned int height, unsigned int multiSamples, float clearColor[4], std::wstring framebufferName)
{
	this->framebufferFormat = format;
	this->multisamples = multiSamples;

	// Description of render target resource.
	D3D12_RESOURCE_DESC resourceDesc = {};
	ZeroMemory(&resourceDesc, sizeof(resourceDesc));
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDesc.Alignment = 0;
	resourceDesc.SampleDesc.Count = multiSamples;
	resourceDesc.SampleDesc.Quality = 0;
	resourceDesc.MipLevels = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.Width = width;
	resourceDesc.Height = height;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	resourceDesc.Format = format;

	// Describe the clear value.
	D3D12_CLEAR_VALUE clearVal = {};
	clearVal.Color[0] = clearColor[0];
	clearVal.Color[1] = clearColor[1];
	clearVal.Color[2] = clearColor[2];
	clearVal.Color[3] = clearColor[3];
	clearVal.Format = format;

	InitializeResource(&this->renderTargetBuffer, device, &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), &resourceDesc, D3D12_RESOURCE_STATE_RENDER_TARGET, &clearVal);
	this->renderTargetBuffer->SetName((framebufferName + L" Render Buffer").c_str());

	// Describe the render target view.
	ZeroMemory(&this->rtvDesc, sizeof(this->rtvDesc));
	this->rtvDesc.Texture2D.MipSlice = 0;
	this->rtvDesc.Texture2D.PlaneSlice = 0;
	this->rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMS;
	this->rtvDesc.Format = format;

	// Description of shader resource.
	ZeroMemory(&resourceDesc, sizeof(resourceDesc));
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDesc.Alignment = 0;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.SampleDesc.Quality = 0;
	resourceDesc.Width = width;
	resourceDesc.Height = height;
	resourceDesc.MipLevels = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	resourceDesc.Format = format;

	InitializeResource(&this->textureBuffer, device, &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr);
	this->textureBuffer->SetName((framebufferName + L"Shader Resource").c_str());

	// Describe shader resource view.
	ZeroMemory(&this->srvDesc, sizeof(this->srvDesc));
	this->srvDesc.Texture2D.MipLevels = 1;
	this->srvDesc.Texture2D.MostDetailedMip = 0;
	this->srvDesc.Texture2D.PlaneSlice = 0;
	this->srvDesc.Format = format;
	this->srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	this->srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
}

void RenderFramebuffer::SetFramebufferToRTVHandle(Microsoft::WRL::ComPtr<ID3D12Device5> device, D3D12_CPU_DESCRIPTOR_HANDLE handle)
{
	device->CreateRenderTargetView(this->renderTargetBuffer.Get(), &this->rtvDesc, handle);
}

void RenderFramebuffer::SetFramebufferToSRVHandle(Microsoft::WRL::ComPtr<ID3D12Device5> device, D3D12_CPU_DESCRIPTOR_HANDLE handle)
{
	device->CreateShaderResourceView(this->textureBuffer.Get(), &this->srvDesc, handle);
}

void RenderFramebuffer::CopyResource(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList)
{
	// Record the previous and next state of the resources.
	D3D12_RESOURCE_STATES previousFromResourceState = D3D12_RESOURCE_STATE_RENDER_TARGET;
	D3D12_RESOURCE_STATES previousToResourceState = D3D12_RESOURCE_STATE_GENERIC_READ;

	D3D12_RESOURCE_STATES copyFromResourceState;
	D3D12_RESOURCE_STATES copyToResourceState;

	if (this->multisamples > 1)
	{
		copyFromResourceState = D3D12_RESOURCE_STATE_RESOLVE_SOURCE;
		copyToResourceState = D3D12_RESOURCE_STATE_RESOLVE_DEST;
	}
	else
	{
		copyFromResourceState = D3D12_RESOURCE_STATE_COPY_SOURCE;
		copyToResourceState = D3D12_RESOURCE_STATE_COPY_DEST;
	}

	// Transition the state of the resource.
	TransitionResourceState(this->renderTargetBuffer, commandList, previousFromResourceState, copyFromResourceState);
	TransitionResourceState(this->textureBuffer, commandList, previousToResourceState, copyToResourceState);

	// Transfer the buffer content based on the number of multi samples on the render buffer.
	if (this->multisamples > 1)
	{
		commandList->ResolveSubresource(this->textureBuffer.Get(), 0, this->renderTargetBuffer.Get(), 0, this->framebufferFormat);
	}
	else
	{
		commandList->CopyResource(this->textureBuffer.Get(), this->renderTargetBuffer.Get());
	}

	// Transition the resources back to its default states.
	TransitionResourceState(this->renderTargetBuffer, commandList, copyFromResourceState, previousFromResourceState);
	TransitionResourceState(this->textureBuffer, commandList, copyToResourceState, previousToResourceState);

}
