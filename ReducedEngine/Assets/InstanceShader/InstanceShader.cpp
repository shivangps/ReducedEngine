#include "InstanceShader.h"

void InstanceShader::Initialize(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int numRT, DXGI_FORMAT renderTargetFormats[], DXGI_FORMAT depthStencilFormat, unsigned int samples)
{

	this->CreateShaderFromFile("Assets/InstanceShader/instance_vertex_shader.hlsl", "Assets/InstanceShader/instance_pixel_shader.hlsl");

	this->CreateConstantBufferTable(device, 1);

	{
		CD3DX12_ROOT_PARAMETER1 rootParameter;
		rootParameter.InitAsShaderResourceView(this->numberOfSRV++ , 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_ALL);
		this->rootParameters.push_back(rootParameter);
		this->CreateRootSignature(device);
		this->numberOfSlots++;
	}

	D3D12_INPUT_ELEMENT_DESC inputLayout[]
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "SV_InstanceID", 0, DXGI_FORMAT_R16_UINT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
	};

	this->CreateGraphicsPipelineState(device, numRT, renderTargetFormats, depthStencilFormat, samples, { inputLayout, _countof(inputLayout) });

}
