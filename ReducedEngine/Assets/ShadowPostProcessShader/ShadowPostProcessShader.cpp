#include "ShadowPostProcessShader.h"

void ShadowPostProcessShader::Initialize(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int numRT, DXGI_FORMAT renderTargetFormats[], unsigned int samples)
{
	this->CreateShaderFromFile("Assets/ShadowPostProcessShader/shadow_post_process_vertex_shader.hlsl", "Assets/ShadowPostProcessShader/shadow_post_process_pixel_shader.hlsl");

	this->CreateTextureTable(device, 1);
	this->CreateConstantBufferTable(device, 1);

	D3D12_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	DXGI_FORMAT formats[1] = { this->format };
	this->CreateGraphicsPipelineState(device, _countof(formats), formats, 1, { inputLayout, _countof(inputLayout) });
}
