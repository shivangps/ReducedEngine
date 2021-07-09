#include "DisplayShader.h"
#include "..//..//ErrorLogger.h"

void DisplayShader::Initialize(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int numRT, DXGI_FORMAT renderTargetFormats[], DXGI_FORMAT depthStencilFormat, unsigned int samples)
{
	this->CreateShaderFromFile("Assets/DisplayShader/display_vertex_shader.hlsl", "Assets/DisplayShader/display_pixel_shader.hlsl");

	this->CreateTextureTable(device, 1);

	D3D12_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	this->CreateGraphicsPipelineState(device, numRT, renderTargetFormats, depthStencilFormat, samples, { inputLayout, _countof(inputLayout) });
}
