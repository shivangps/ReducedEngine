#include "SSAOBlurShader.h"

void SSAOBlurShader::Initialize(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int numRT, DXGI_FORMAT renderTargetFormats[], unsigned int samples)
{
	this->CreateShaderFromFile("Assets/SSAOBlurShader/ssao_blur_vertex_shader.hlsl", "Assets/SSAOBlurShader/ssao_blur_pixel_shader.hlsl");

	this->CreateTextureTable(device, 1);
	this->CreateConstantBufferTable(device, 1);

	this->rootSignature->SetName(L"SSAO Blur Root Signature");

	D3D12_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	DXGI_FORMAT formatArray[] = { this->pipelineFormat };

	this->CreateGraphicsPipelineState(device, _countof(formatArray), formatArray, 1, { inputLayout, _countof(inputLayout) });
}