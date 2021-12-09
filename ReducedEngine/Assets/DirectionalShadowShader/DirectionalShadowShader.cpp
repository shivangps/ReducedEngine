#include "DirectionalShadowShader.h"

void DirectionalShadowShader::Initialize(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int numRT, DXGI_FORMAT renderTargetFormats[], unsigned int samples)
{
	this->CreateShaderFromFile("Assets/DirectionalShadowShader/directional_shadow_vertex_shader.hlsl", "Assets/DirectionalShadowShader/directional_shadow_pixel_shader.hlsl");

	// Making the texture sampler borderless.
	this->samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	this->samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	this->samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	this->samplerDesc.Filter = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;

	this->CreateConstantBufferTable(device, 1);
	this->CreateTextureTable(device, 3);

	D3D12_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	DXGI_FORMAT formats[1] = { this->format };
	this->CreateGraphicsPipelineState(device, _countof(formats), formats, 1, { inputLayout, _countof(inputLayout) });
}
