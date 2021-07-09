#include "CubemapShader.h"

#include "../ErrorLogger.h"

void CubemapShader::Initialize(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int numRT, DXGI_FORMAT renderTargetFormats[], DXGI_FORMAT depthStencilFormat, unsigned int samples)
{
	this->CreateShaderFromFile("Assets/CubemapShader/cubemap_vertex_shader.hlsl", "Assets/CubemapShader/cubemap_pixel_shader.hlsl");

	ZeroMemory(&this->samplerDesc, sizeof(this->samplerDesc));
	this->samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	this->samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	this->samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	this->samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	this->samplerDesc.MipLODBias = 0;
	this->samplerDesc.MaxAnisotropy = 0;
	this->samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	this->samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	this->samplerDesc.MinLOD = 0.0f;
	this->samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
	this->samplerDesc.ShaderRegister = 0;
	this->samplerDesc.RegisterSpace = 0;
	this->samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	this->CreateTextureTable(device, 1);
	this->CreateConstantBufferTable(device, 1);

	D3D12_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	// Create pipeline state.
	{
		D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc = {};
		ZeroMemory(&pipelineDesc, sizeof(pipelineDesc));
		pipelineDesc.InputLayout = { inputLayout, _countof(inputLayout) };
		pipelineDesc.pRootSignature = this->rootSignature.Get();
		pipelineDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
		pipelineDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());
		pipelineDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
		pipelineDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		pipelineDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		pipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		pipelineDesc.SampleMask = UINT_MAX;
		pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		pipelineDesc.NumRenderTargets = numRT;
		for (unsigned int i = 0; i < numRT; i++)
		{
			pipelineDesc.RTVFormats[i] = renderTargetFormats[i];
		}
		pipelineDesc.SampleDesc.Count = samples;
		pipelineDesc.SampleDesc.Quality = 0;
		pipelineDesc.DSVFormat = depthStencilFormat;

		HRESULT HR = device->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&this->graphicsPipelineState));
		ExitOnError(HR, "Failed to create graphics pipeline for cubemap.");
	}
}
