#include "WireframeShader.h"
#include "../../../../ErrorLogger.h"

void WireframeShader::Initialize(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int numRT, DXGI_FORMAT renderTargetFormats[], DXGI_FORMAT depthStencilFormat, unsigned int samples)
{
	this->CreateShaderFromFile("Assets/2D_Assets/Shaders/WireframeShader/wireframe_vertex_shader.hlsl", "Assets/2D_Assets/Shaders/WireframeShader/wireframe_pixel_shader.hlsl");

	this->CreateConstantBufferTable(device, 2);

	D3D12_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	HRESULT HR;

	// Describe the rasterization process.
	D3D12_RASTERIZER_DESC rasterizerDesc = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
	rasterizerDesc.FillMode = D3D12_FILL_MODE_WIREFRAME;

	// Descride the pipeline state.
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc = {};
	ZeroMemory(&pipelineDesc, sizeof(pipelineDesc));
	pipelineDesc.InputLayout = { inputLayout, _countof(inputLayout)};
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
