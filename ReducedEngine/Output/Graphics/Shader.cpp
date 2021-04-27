#include "Shader.h"
#include "..//..//ErrorLogger.h"
#include <d3dcompiler.h>

void Shader::CreateShaderFromFile(std::string vertexFilePath, std::string pixelFilePath)
{
	HRESULT HR;

#if defined(_DEBUG)
	// Enable shader debugging with graphics debugging tools.
	unsigned int compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	unsigned int compileFlags = 0;
#endif

	std::wstring fileName_wide;
	fileName_wide = StringToWide(vertexFilePath);
	HR = D3DCompileFromFile(fileName_wide.c_str(), nullptr, nullptr, "main", "vs_5_0", compileFlags, 0, &this->vertexShader, nullptr);
	ExitOnError(HR, "Failed to create vertex shader from file.");

	fileName_wide = StringToWide(pixelFilePath);
	HR = D3DCompileFromFile(fileName_wide.c_str(), nullptr, nullptr, "main", "ps_5_0", compileFlags, 0, &this->pixelShader, nullptr);
	ExitOnError(HR, "Failed to create pixel shader from file.");
}

void Shader::CreateDefaultSampler()
{
	this->samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
	this->samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	this->samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	this->samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	this->samplerDesc.MipLODBias = 10;
	this->samplerDesc.MaxAnisotropy = 0;
	this->samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	this->samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	this->samplerDesc.MinLOD = 0.0f;
	this->samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
	this->samplerDesc.ShaderRegister = 0;
	this->samplerDesc.RegisterSpace = 0;
	this->samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
}

void Shader::CreateRootSignature(Microsoft::WRL::ComPtr<ID3D12Device5> device)
{
	HRESULT HR;

	this->rootSigDesc.Init_1_1(this->rootParameters.size(), &this->rootParameters[0], 1, &this->samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	Microsoft::WRL::ComPtr<ID3DBlob> signature;
	Microsoft::WRL::ComPtr<ID3DBlob> error;
	HR = D3DX12SerializeVersionedRootSignature(&this->rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1_1, &signature, &error);
	ExitOnError(HR, "Failed to serialize root signature.");

	HR = device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(this->rootSignature.GetAddressOf()));
	ExitOnError(HR, "Failed to create the root signature.");

	signature->Release();
}

void Shader::CreateDescriptorTable(Microsoft::WRL::ComPtr<ID3D12Device5> device, D3D12_DESCRIPTOR_RANGE_TYPE type, unsigned int slot)
{
	CD3DX12_DESCRIPTOR_RANGE1* pRange;
	pRange = new CD3DX12_DESCRIPTOR_RANGE1;
	pRange->Init(type, 1, slot, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
	CD3DX12_ROOT_PARAMETER1 rootParameter;
	rootParameter.InitAsDescriptorTable(1, pRange, D3D12_SHADER_VISIBILITY_ALL);
	this->rootParameters.push_back(rootParameter);
	this->CreateRootSignature(device);
}

void Shader::CreateConstantBufferTable(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int numOfConstants)
{
	for (unsigned int i = 0; i < numOfConstants; i++)
	{
		this->CreateDescriptorTable(device, D3D12_DESCRIPTOR_RANGE_TYPE_CBV, this->numberOfCBV++);
		this->numberOfSlots++;
	}
}

void Shader::CreateTextureTable(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int numOfTextures)
{
	for (unsigned int i = 0; i < numOfTextures; i++)
	{
		this->CreateDescriptorTable(device, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, this->numberOfSRV++);
		this->numberOfSlots++;
	}
}

void Shader::CreateGraphicsPipelineState(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int numRT, DXGI_FORMAT renderTargetFormats[], DXGI_FORMAT depthStencilFormat, 
	unsigned int samples, D3D12_INPUT_LAYOUT_DESC inputElementDesc)
{
	HRESULT HR;

	// Describe the rasterization process.
	D3D12_RASTERIZER_DESC rasterizerDesc = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

	// Descride the pipeline state.
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc = {};
	ZeroMemory(&pipelineDesc, sizeof(pipelineDesc));
	pipelineDesc.InputLayout = inputElementDesc;
	pipelineDesc.pRootSignature = this->rootSignature.Get();
	pipelineDesc.VS = CD3DX12_SHADER_BYTECODE(this->vertexShader.Get());
	pipelineDesc.PS = CD3DX12_SHADER_BYTECODE(this->pixelShader.Get());
	pipelineDesc.RasterizerState = rasterizerDesc;
	pipelineDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	pipelineDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
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

	HR = device->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(this->graphicsPipelineState.GetAddressOf()));
	ExitOnError(HR, "Failed to create graphics pipeline state.");
}

void Shader::SetShaderForRender(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList)
{
	commandList->SetPipelineState(this->graphicsPipelineState.Get());
	commandList->SetGraphicsRootSignature(this->rootSignature.Get());
}
