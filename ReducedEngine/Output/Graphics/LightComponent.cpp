#include "LightComponent.h"
#include "../../Assets/DirectionalShadowDepthShader/DirectionalShadowDepthShader.h"
#include "../Output/Output.h"

bool LightComponent::shadowShaderInitialized = false;
bool LightComponent::initializedDirectionalShadowShader = false;
bool LightComponent::initializedShadowPostProcessShader = false;

void LightComponent::BuildLightConstantBuffer(Microsoft::WRL::ComPtr<ID3D12Device5> device)
{
	// Create Resource.
	unsigned int size = sizeof(this->DLCharacteristics);
	InitializeResource(&this->DLCConstantBuffer, device, &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), &CD3DX12_RESOURCE_DESC::Buffer(GetAggregateSize(size)), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr);
	this->DLCConstantBuffer->SetName(L"Directional Light Characteristics Constant Buffer Resource");

	// Create constant buffer view.
	D3D12_CONSTANT_BUFFER_VIEW_DESC constantBufferViewDesc = {};
	constantBufferViewDesc.BufferLocation = this->DLCConstantBuffer->GetGPUVirtualAddress();
	constantBufferViewDesc.SizeInBytes = GetAggregateSize(size);
	device->CreateConstantBufferView(&constantBufferViewDesc, this->lightDescriptorHeap.GetCPUHandle(Slot::directionalLightCharacteristics));

	// Create data pointer to cbv memory space.
	CD3DX12_RANGE readRange = {};
	this->DLCConstantBuffer->Map(0, &readRange, reinterpret_cast<void**>(&this->pLightCharacteristicsCBV));
	this->DLCConstantBuffer->Unmap(0, nullptr);
}

Transform LightComponent::GetTransform()
{
	return *this->transform;
}

void LightComponent::Initialize(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int width, unsigned int height)
{
	// Create a viewport for shadow resolution.
	this->shadowViewport.TopLeftX = 0.0f;
	this->shadowViewport.TopLeftY = 0.0f;
	this->shadowViewport.Width = (float)this->shadowWidth;
	this->shadowViewport.Height = (float)this->shadowHeight;

	this->shadowViewport.MinDepth = 0.0f;
	this->shadowViewport.MaxDepth = 1.0f;

	// Create a clipping rectangle for shadow resolution.
	this->shadowClippingRect.left = 0.0f;
	this->shadowClippingRect.top = 0.0f;
	this->shadowClippingRect.right = (float)this->shadowWidth;
	this->shadowClippingRect.bottom = (float)this->shadowHeight;

	this->lightDescriptorHeap.Initialize(device, Slot::Size, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
	this->shadowHeap.Initialize(device, 1, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);

	this->BuildLightConstantBuffer(device);

	this->DLCharacteristics.shadowWidth = this->shadowWidth;
	this->DLCharacteristics.shadowHeight = this->shadowHeight;
	this->shadowDepth.Initialize(device, this->shadowWidth, this->shadowHeight, 1, L"Shadow Depth Buffer for Directional Light");
	this->shadowDepth.SetDepthBufferToDSVHandle(device, this->shadowHeap.GetCPUHandle(0));

	if (!this->shadowShaderInitialized)
	{
		DirectionalShadowDepthShader::GetInstance()->Initialize(device, 0, nullptr, this->shadowDepth.GetRenderTargetDepthFormat(), 1);
		shadowShaderInitialized = true;
	}
	this->shadowDepthShader = DirectionalShadowDepthShader::GetInstance();

	// Initilize the shadow.
	this->InitializeShadowRender(device, width, height);
}

void LightComponent::InitializeShadowRender(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int width, unsigned int height)
{
	// MAIN SHADOW RENDER.

	// Initialize the shader once.
	if (!this->initializedDirectionalShadowShader)
	{
		this->shadowRenderShader->Initialize(device, 0, nullptr, 0);
		this->initializedDirectionalShadowShader = true;
	}

	// Initialize all the resources.
	this->shadowRenderSrvHeap.Initialize(device, SHADOW_RENDER_SLOT::size, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
	this->shadowRenderRtvHeap.Initialize(device, 1, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
	this->shadowRenderBuffer.Initialize(device, this->shadowRenderFormat, width, height, 1, this->shadowClearColor, L"Shadow Render");

	this->shadowRenderBuffer.SetFramebufferToRTVHandle(device, this->shadowRenderRtvHeap.GetCPUHandle(0));
	this->shadowDepth.SetDepthBufferToSRVHandle(device, this->shadowRenderSrvHeap.GetCPUHandle(SHADOW_RENDER_SLOT::shadowDepth_srv));

	this->shadowRenderDataCbv.shadowWidth = this->shadowWidth;
	this->shadowRenderDataCbv.shadowHeight = this->shadowHeight;

	// Shadow render constant buffer initialization.
	{
		unsigned int size = sizeof(this->shadowRenderDataCbv);
		InitializeResource(&this->shadowRenderConstantBuffer, device, &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), &CD3DX12_RESOURCE_DESC::Buffer(size), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr);
		this->shadowRenderConstantBuffer->SetName(L"Directional Light Shadow Render Constant Buffer");

		D3D12_CONSTANT_BUFFER_VIEW_DESC constantBufferViewDesc = {};
		constantBufferViewDesc.BufferLocation = this->shadowRenderConstantBuffer->GetGPUVirtualAddress();
		constantBufferViewDesc.SizeInBytes = GetAggregateSize(size);
		device->CreateConstantBufferView(&constantBufferViewDesc, this->shadowRenderSrvHeap.GetCPUHandle(SHADOW_RENDER_SLOT::cbv));
		
		CD3DX12_RANGE readRange = {};
		this->shadowRenderConstantBuffer->Map(0, &readRange, reinterpret_cast<void**>(&this->pShadowRenderData));
		this->shadowRenderConstantBuffer->Unmap(0, nullptr);
	}

	// SHADOW POST PROCESS.

	// Initialize the shader once.
	if (!this->initializedShadowPostProcessShader)
	{
		this->postProcessShader->Initialize(device, 0, nullptr, 0);
		this->initializedShadowPostProcessShader = true;
	}

	// Initialize all the resources.
	this->shadowPPSrvHeap.Initialize(device, SHADOW_PP_SLOT::ppSize, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
	this->shadowPPRtvHeap.Initialize(device, 1, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
	this->shadowPPRenderBuffer.Initialize(device, this->shadowPPFormat, width, height, 1, this->shadowPPClearColor, L"Shadow Post Process Buffer");

	this->shadowPPRenderBuffer.SetFramebufferToRTVHandle(device, this->shadowPPRtvHeap.GetCPUHandle(0));
	this->shadowRenderBuffer.SetFramebufferToSRVHandle(device, this->shadowPPSrvHeap.GetCPUHandle(SHADOW_PP_SLOT::ppinputTexture_srv));

	// Shadow post processing constant buffer initialization.
	{
		unsigned int size = sizeof(this->shadowPPData);
		InitializeResource(&this->shadowPPConstantBuffer, device, &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), &CD3DX12_RESOURCE_DESC::Buffer(size), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr);
		this->shadowPPConstantBuffer->SetName(L"Shadow Post Processing Constant Buffer");

		D3D12_CONSTANT_BUFFER_VIEW_DESC constantBufferViewDesc = {};
		constantBufferViewDesc.BufferLocation = this->shadowPPConstantBuffer->GetGPUVirtualAddress();
		constantBufferViewDesc.SizeInBytes = GetAggregateSize(size);
		device->CreateConstantBufferView(&constantBufferViewDesc, this->shadowPPSrvHeap.GetCPUHandle(SHADOW_PP_SLOT::ppCbv));

		CD3DX12_RANGE readRange = {};
		this->shadowPPConstantBuffer->Map(0, &readRange, reinterpret_cast<void**>(&this->pShadowPPCBData));
		this->shadowPPConstantBuffer->Unmap(0, nullptr);

		this->shadowPPData.width = width;
		this->shadowPPData.height = height;
		memcpy(this->pShadowPPCBData, &this->shadowPPData, sizeof(this->shadowPPData));
	}

	// Applying the end result of the shadow render to the input textures of the light shader.
	this->shadowPPRenderBuffer.SetFramebufferToSRVHandle(device, this->lightDescriptorHeap.GetCPUHandle(Slot::shadowRenderSlot));
}

void LightComponent::RenderShadow(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList)
{
	// MAIN SHADOW RENDERING.
	// Update the light space matrix in the constant buffer.
	this->shadowRenderDataCbv.lightSpaceMatrix = this->lightSpaceMatrix;
	memcpy(this->pShadowRenderData, &this->shadowRenderDataCbv, sizeof(this->shadowRenderDataCbv));

	this->shadowRenderShader->SetShaderForRender(commandList);

	commandList->ClearRenderTargetView(this->shadowRenderRtvHeap.GetCPUHandle(0), this->shadowClearColor, 0, nullptr);

	commandList->OMSetRenderTargets(1, &this->shadowRenderRtvHeap.GetCPUHandle(0), false, nullptr);

	ID3D12DescriptorHeap* ppHeaps[] = { this->shadowRenderSrvHeap.Get() };
	commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	commandList->SetGraphicsRootDescriptorTable(SHADOW_RENDER_SLOT::cbv, this->shadowRenderSrvHeap.GetGPUHandle(SHADOW_RENDER_SLOT::cbv));
	commandList->SetGraphicsRootDescriptorTable(SHADOW_RENDER_SLOT::fragmentPosition_srv, this->shadowRenderSrvHeap.GetGPUHandle(SHADOW_RENDER_SLOT::fragmentPosition_srv));
	commandList->SetGraphicsRootDescriptorTable(SHADOW_RENDER_SLOT::shadowDepth_srv, this->shadowRenderSrvHeap.GetGPUHandle(SHADOW_RENDER_SLOT::shadowDepth_srv));
	commandList->SetGraphicsRootDescriptorTable(SHADOW_RENDER_SLOT::normal_srv, this->shadowRenderSrvHeap.GetGPUHandle(SHADOW_RENDER_SLOT::normal_srv));

	this->quad->Draw(commandList);

	this->shadowRenderBuffer.CopyResource(commandList);

	// SHADOW POST PROCESSING EFFECTS.

	this->postProcessShader->SetShaderForRender(commandList);

	commandList->ClearRenderTargetView(this->shadowPPRtvHeap.GetCPUHandle(0), this->shadowPPClearColor, 0, nullptr);

	commandList->OMSetRenderTargets(1, &this->shadowPPRtvHeap.GetCPUHandle(0), false, nullptr);

	ppHeaps[0] = this->shadowPPSrvHeap.Get();
	commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	commandList->SetGraphicsRootDescriptorTable(SHADOW_PP_SLOT::ppinputTexture_srv, this->shadowPPSrvHeap.GetGPUHandle(SHADOW_PP_SLOT::ppinputTexture_srv));
	commandList->SetGraphicsRootDescriptorTable(SHADOW_PP_SLOT::ppCbv, this->shadowPPSrvHeap.GetGPUHandle(SHADOW_PP_SLOT::ppCbv));

	this->quad->Draw(commandList);

	this->shadowPPRenderBuffer.CopyResource(commandList);
}

void LightComponent::SetLightForRendering(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList, Camera camera)
{
	this->UpdateConstantBuffer(camera);

	this->directionalLightShader->SetShaderForRender(commandList);

	ID3D12DescriptorHeap* ppHeaps[] = { this->lightDescriptorHeap.Get() };
	commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	commandList->SetGraphicsRootDescriptorTable(DirectionalLightShader::Slot::directionalLightCharacteristics, this->lightDescriptorHeap.GetGPUHandle(Slot::directionalLightCharacteristics));
	commandList->SetGraphicsRootDescriptorTable(DirectionalLightShader::Slot::fragmentPosition, this->lightDescriptorHeap.GetGPUHandle(Slot::fragmentPosition));
	commandList->SetGraphicsRootDescriptorTable(DirectionalLightShader::Slot::normal, this->lightDescriptorHeap.GetGPUHandle(Slot::normal));
	commandList->SetGraphicsRootDescriptorTable(DirectionalLightShader::Slot::albedoSpecular, this->lightDescriptorHeap.GetGPUHandle(Slot::albedoSpecular));
	commandList->SetGraphicsRootDescriptorTable(DirectionalLightShader::Slot::shadowDepth, this->lightDescriptorHeap.GetGPUHandle(Slot::shadowRenderSlot));
	commandList->SetGraphicsRootDescriptorTable(DirectionalLightShader::Slot::ssao, this->lightDescriptorHeap.GetGPUHandle(Slot::ssao));

	this->quad->Draw(commandList);
}

void LightComponent::UpdateConstantBuffer(Camera camera)
{
	Vector3 lightDirection = this->transform->GetLocalUp();
	this->DLCharacteristics.direction = DirectX::XMFLOAT4(lightDirection.X(), lightDirection.Y(), lightDirection.Z(), 0.0f);

	Vector3 cameraPosition = camera.GetTransform().GetGlobalPostion();
	this->DLCharacteristics.viewDirection = DirectX::XMFLOAT4(cameraPosition.X(), cameraPosition.Y(), cameraPosition.Z(), 0.0f);

	this->DLCharacteristics.lightSpaceMatrix = this->lightSpaceMatrix;

	memcpy(this->pLightCharacteristicsCBV, &this->DLCharacteristics, sizeof(this->DLCharacteristics));
}

void LightComponent::SetFramebufferToFragmentPositionHandle(Microsoft::WRL::ComPtr<ID3D12Device5> device, RenderFramebuffer fragmentPositionFramebuffer)
{
	fragmentPositionFramebuffer.SetFramebufferToSRVHandle(device, this->lightDescriptorHeap.GetCPUHandle(Slot::fragmentPosition));
	fragmentPositionFramebuffer.SetFramebufferToSRVHandle(device, this->shadowRenderSrvHeap.GetCPUHandle(SHADOW_RENDER_SLOT::fragmentPosition_srv));
}

void LightComponent::SetFramebufferToNormalHandle(Microsoft::WRL::ComPtr<ID3D12Device5> device, RenderFramebuffer normalFramebuffer)
{
	normalFramebuffer.SetFramebufferToSRVHandle(device, this->lightDescriptorHeap.GetCPUHandle(Slot::normal));
	normalFramebuffer.SetFramebufferToSRVHandle(device, this->shadowRenderSrvHeap.GetCPUHandle(SHADOW_RENDER_SLOT::normal_srv));
}

void LightComponent::SetFramebufferToAlbedoSpecular(Microsoft::WRL::ComPtr<ID3D12Device5> device, RenderFramebuffer albedoSpecFramebuffer)
{
	albedoSpecFramebuffer.SetFramebufferToSRVHandle(device, this->lightDescriptorHeap.GetCPUHandle(Slot::albedoSpecular));
}

void LightComponent::SetFramebufferToSSAO(Microsoft::WRL::ComPtr<ID3D12Device5> device, RenderFramebuffer ssaoFramenbuffer)
{
	ssaoFramenbuffer.SetFramebufferToSRVHandle(device, this->lightDescriptorHeap.GetCPUHandle(Slot::ssao));
}

void LightComponent::SetAmbientColor(Vector3 ambient)
{
	this->DLCharacteristics.ambient = DirectX::XMFLOAT4(ambient.X(), ambient.Y(), ambient.Z(), 0.0f);
}

void LightComponent::SetDiffuseColor(Vector3 diffuse)
{
	this->DLCharacteristics.diffuse = DirectX::XMFLOAT4(diffuse.X(), diffuse.Y(), diffuse.Z(), 0.0f);
}

void LightComponent::SetSpecularColor(Vector3 specular)
{
	this->DLCharacteristics.specular = DirectX::XMFLOAT4(specular.X(), specular.Y(), specular.Z(), 0.0f);
}

void LightComponent::RenderSceneToShadowDepth(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList, 
	unsigned int numberOfRenderTargets, 
	D3D12_CPU_DESCRIPTOR_HANDLE* pRenderTargetHandle, 
	bool RTsSingleHandleToDescriptorRange, 
	D3D12_CPU_DESCRIPTOR_HANDLE* pDepthStencilTargetHandle, 
	RenderList* renderComponentList, Vector3 shadowPosition,
	D3D12_VIEWPORT* currentSetViewport,
	D3D12_RECT* currentSetClipRect)
{
	commandList->RSSetViewports(1, &this->shadowViewport);
	commandList->RSSetScissorRects(1, &this->shadowClippingRect);

	Matrix4 view = DirectX::XMMatrixLookAtLH(shadowPosition.GetVector(), this->transform->GetLocalDown().GetVector(), this->transform->GetLocalUp().GetVector());
	Matrix4 projection = DirectX::XMMatrixOrthographicLH(5.0f, 5.0f, 0.1f, 10.0f);

	this->lightSpaceMatrix = view * projection;

	commandList->ClearDepthStencilView(this->shadowHeap.GetCPUHandle(0), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	commandList->OMSetRenderTargets(0, nullptr, true, &this->shadowHeap.GetCPUHandle(0));

	this->shadowDepthShader->SetShaderForRender(commandList);

	renderComponentList->DrawAllComponentsForShadow(commandList, this->lightSpaceMatrix);

	commandList->OMSetRenderTargets(numberOfRenderTargets, pRenderTargetHandle, RTsSingleHandleToDescriptorRange, pDepthStencilTargetHandle);

	this->shadowDepth.CopyResource(commandList);

	commandList->RSSetViewports(1, currentSetViewport);
	commandList->RSSetScissorRects(1, currentSetClipRect);
}
