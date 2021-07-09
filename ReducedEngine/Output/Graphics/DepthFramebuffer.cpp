#include "DepthFramebuffer.h"

void DepthFramebuffer::Initialize(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int width, unsigned int height, unsigned int multiSamples, std::wstring depthBufferName)
{
	this->multiSamples = multiSamples;

	HRESULT HR;

	// Describe the depth resource.
	D3D12_RESOURCE_DESC depthResourceDesc;
	ZeroMemory(&depthResourceDesc, sizeof(depthResourceDesc));
	depthResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthResourceDesc.Alignment = 0;
	depthResourceDesc.SampleDesc.Count = multiSamples;
	depthResourceDesc.SampleDesc.Quality = 0;
	depthResourceDesc.MipLevels = 1;
	depthResourceDesc.Format = this->depthRtFormat;
	depthResourceDesc.DepthOrArraySize = 1;
	depthResourceDesc.Width = width;
	depthResourceDesc.Height = height;
	depthResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	depthResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE clearVal = { this->depthRtFormat, 1.0f };

	// Create the depth buffer.
	InitializeResource(&this->depthStencilBuffer, device, &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), &depthResourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &clearVal);
	this->depthStencilBuffer->SetName((depthBufferName + L" Write Depth Buffer").c_str());

	// Create shader resource.
	depthResourceDesc.SampleDesc.Count = 1;
	InitializeResource(&this->textureBuffer, device, &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), &depthResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, &clearVal);
	this->textureBuffer->SetName((depthBufferName + L" Texture Depth Buffer").c_str());

	// Describe the depth resource view.
	ZeroMemory(&this->dsvDesc, sizeof(this->dsvDesc));
	this->dsvDesc.Texture2D.MipSlice = 0;
	this->dsvDesc.Format = this->depthRtFormat;
	this->dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMS;
	this->dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

	// Describe the shader resource.
	ZeroMemory(&this->srvDesc, sizeof(this->srvDesc));
	this->srvDesc.Texture2D.MipLevels = 1;
	this->srvDesc.Texture2D.MostDetailedMip = 0;
	this->srvDesc.Format = this->depthSrFormat;
	this->srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	this->srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
}

DXGI_FORMAT DepthFramebuffer::GetRenderTargetDepthFormat()
{
	return this->depthRtFormat;
}

void DepthFramebuffer::SetDepthBufferToDSVHandle(Microsoft::WRL::ComPtr<ID3D12Device5> device, D3D12_CPU_DESCRIPTOR_HANDLE handle)
{
	device->CreateDepthStencilView(this->depthStencilBuffer.Get(), &this->dsvDesc, handle);
}

void DepthFramebuffer::SetDepthBufferToSRVHandle(Microsoft::WRL::ComPtr<ID3D12Device5> device, D3D12_CPU_DESCRIPTOR_HANDLE handle)
{
	device->CreateShaderResourceView(this->textureBuffer.Get(), &this->srvDesc, handle);
}

void DepthFramebuffer::CopyResource(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList)
{
	// Record the previous and next state of the resources.
	D3D12_RESOURCE_STATES previousFromResourceState = D3D12_RESOURCE_STATE_DEPTH_WRITE;
	D3D12_RESOURCE_STATES previousToResourceState = D3D12_RESOURCE_STATE_GENERIC_READ;

	D3D12_RESOURCE_STATES copyFromResourceState;
	D3D12_RESOURCE_STATES copyToResourceState;

	if (this->multiSamples > 1)
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
	TransitionResourceState(this->depthStencilBuffer, commandList, previousFromResourceState, copyFromResourceState);
	TransitionResourceState(this->textureBuffer, commandList, previousToResourceState, copyToResourceState);

	// Transfer the buffer content based on the number of multi samples on the render buffer.
	if (this->multiSamples > 1)
	{
		commandList->ResolveSubresource(this->textureBuffer.Get(), 0, this->depthStencilBuffer.Get(), 0, this->depthSrFormat);
	}
	else
	{
		commandList->CopyResource(this->textureBuffer.Get(), this->depthStencilBuffer.Get());
	}

	// Transition the resources back to its default states.
	TransitionResourceState(this->depthStencilBuffer, commandList, copyFromResourceState, previousFromResourceState);
	TransitionResourceState(this->textureBuffer, commandList, copyToResourceState, previousToResourceState);
}
