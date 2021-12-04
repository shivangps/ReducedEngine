#include "SSAOShader.h"
#include "../../ErrorLogger.h"

void SSAOShader::Initialize(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int numRT, DXGI_FORMAT renderTargetFormats[], DXGI_FORMAT depthStencilFormat, unsigned int samples)
{
	this->CreateShaderFromFile("Assets/SSAOShader/ssao_vertex_shader.hlsl", "Assets/SSAOShader/ssao_pixel_shader.hlsl");

	this->samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	this->samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	this->samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;

	this->CreateTextureTable(device, 3);
	this->CreateConstantBufferTable(device, 1);

	{
		CD3DX12_ROOT_PARAMETER1 rootParameter;
		rootParameter.InitAsShaderResourceView(this->numberOfSRV++, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_ALL);
		this->rootParameters.push_back(rootParameter);
		this->CreateRootSignature(device);
		this->numberOfSlots++;
	}

	this->rootSignature->SetName(L"SSAO Root Signature");

	D3D12_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	DXGI_FORMAT pipelineRenderFormat = this->ssaoRenderTargetFormat;

	HRESULT HR;

	// Describe the rasterization process.
	D3D12_RASTERIZER_DESC rasterizerDesc = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;

	// Descride the pipeline state.
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc = {};
	ZeroMemory(&pipelineDesc, sizeof(pipelineDesc));
	pipelineDesc.InputLayout = { inputLayout, _countof(inputLayout) };
	pipelineDesc.pRootSignature = this->rootSignature.Get();
	pipelineDesc.VS = CD3DX12_SHADER_BYTECODE(this->vertexShader.Get());
	pipelineDesc.PS = CD3DX12_SHADER_BYTECODE(this->pixelShader.Get());
	pipelineDesc.RasterizerState = rasterizerDesc;
	pipelineDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	pipelineDesc.SampleMask = UINT_MAX;
	pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	pipelineDesc.NumRenderTargets = 1;
	pipelineDesc.RTVFormats[0] = pipelineRenderFormat;
	pipelineDesc.SampleDesc.Count = samples;
	pipelineDesc.SampleDesc.Quality = 0;

	HR = device->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(this->graphicsPipelineState.GetAddressOf()));
	ExitOnError(HR, "Failed to create graphics pipeline state.");
}

void SSAOShader::Initialize(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int numRT, DXGI_FORMAT renderTargetFormats[], unsigned int samples)
{
	this->CreateShaderFromFile("Assets/SSAOShader/ssao_vertex_shader.hlsl", "Assets/SSAOShader/ssao_pixel_shader.hlsl");

	this->samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	this->samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	this->samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;

	this->CreateTextureTable(device, 3);
	this->CreateConstantBufferTable(device, 1);

	{
		CD3DX12_ROOT_PARAMETER1 rootParameter;
		rootParameter.InitAsShaderResourceView(this->numberOfSRV++, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_ALL);
		this->rootParameters.push_back(rootParameter);
		this->CreateRootSignature(device);
		this->numberOfSlots++;
	}

	D3D12_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	DXGI_FORMAT pipelineRenderFormat = this->ssaoRenderTargetFormat;

	HRESULT HR;

	// Describe the rasterization process.
	D3D12_RASTERIZER_DESC rasterizerDesc = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;

	// Descride the pipeline state.
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc = {};
	ZeroMemory(&pipelineDesc, sizeof(pipelineDesc));
	pipelineDesc.InputLayout = { inputLayout, _countof(inputLayout) };
	pipelineDesc.pRootSignature = this->rootSignature.Get();
	pipelineDesc.VS = CD3DX12_SHADER_BYTECODE(this->vertexShader.Get());
	pipelineDesc.PS = CD3DX12_SHADER_BYTECODE(this->pixelShader.Get());
	pipelineDesc.RasterizerState = rasterizerDesc;
	pipelineDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	pipelineDesc.SampleMask = UINT_MAX;
	pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	pipelineDesc.NumRenderTargets = 1;
	pipelineDesc.RTVFormats[0] = pipelineRenderFormat;
	pipelineDesc.SampleDesc.Count = samples;
	pipelineDesc.SampleDesc.Quality = 0;

	HR = device->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(this->graphicsPipelineState.GetAddressOf()));
	ExitOnError(HR, "Failed to create graphics pipeline state.");
}
