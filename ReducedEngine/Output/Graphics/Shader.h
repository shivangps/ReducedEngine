#pragma once
#include <vector>
#include "d3dx12.h"

// This header contains the base class that loads and defines the Shader.
// It also contains convinence functions for shader initialization.

class Shader
{
public:
	Shader()
	{
		this->CreateDefaultSampler();
	}
	~Shader() {}

protected:
	Microsoft::WRL::ComPtr<ID3DBlob> vertexShader = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> geometryShader = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> pixelShader = nullptr;

	unsigned int numberOfCBV = 0;
	unsigned int numberOfSRV = 0;
	unsigned int numberOfUAV = 0;
	unsigned int numberOfSlots = 0;

	D3D12_STATIC_SAMPLER_DESC samplerDesc = {};
	std::vector<CD3DX12_ROOT_PARAMETER1> rootParameters = {};
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSigDesc = {};
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;

	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState = nullptr;

	// Function to create shader from file in directory.
	void CreateShaderFromFile(std::string vertexFilePath, std::string pixelFilePath);
	// Function to create a default sampler.
	void CreateDefaultSampler();
	// Function to create a root signature.
	void CreateRootSignature(Microsoft::WRL::ComPtr<ID3D12Device5> device);
	// Function to create descriptor table.
	void CreateDescriptorTable(Microsoft::WRL::ComPtr<ID3D12Device5> device, D3D12_DESCRIPTOR_RANGE_TYPE type, unsigned int slot);
	void CreateDescriptorTable(Microsoft::WRL::ComPtr<ID3D12Device5> device, D3D12_DESCRIPTOR_RANGE_TYPE type, unsigned int slot, unsigned int registerSpace);
	// Function to create constant buffer descriptor table.
	void CreateConstantBufferTable(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int numOfConstants);
	// Function to create texture buffer descriptor table.
	void CreateTextureTable(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int numOfTextures);
	void CreateTextureTable(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int numOfTextures, unsigned int registerSpace);
	// Function to create unordered access descriptor table.
	void CreateUnorderedAccessTable(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int numOfUnorderedAccess);
	// Function to create graphics pipeline.
	void CreateGraphicsPipelineState(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int numRT, DXGI_FORMAT renderTargetFormats[], DXGI_FORMAT depthStencilFormat,
		unsigned int samples, D3D12_INPUT_LAYOUT_DESC inputElementDesc);
	void CreateGraphicsPipelineState(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int numRT, DXGI_FORMAT renderTargetFormats[], DXGI_FORMAT depthStencilFormat,
		unsigned int samples, D3D12_INPUT_LAYOUT_DESC inputElementDesc, D3D12_CULL_MODE cullMode);
	void CreateGraphicsPipelineState(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int numRT, DXGI_FORMAT renderTargetFormats[], unsigned int samples, D3D12_INPUT_LAYOUT_DESC inputElementDesc);

public:
	// Function to set the shader for rendering.
	void SetShaderForRender(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList);
	// Virtual functions to initialize the shader.
	virtual void Initialize(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int numRT, DXGI_FORMAT renderTargetFormats[], DXGI_FORMAT depthStencilFormat, unsigned int samples) {}
	virtual void Initialize(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int numRT, DXGI_FORMAT renderTargetFormats[], unsigned int samples) {}
};