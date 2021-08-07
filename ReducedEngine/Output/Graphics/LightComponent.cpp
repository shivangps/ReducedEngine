#include "LightComponent.h"
#include "../../Assets/DirectionalShadowDepthShader/DirectionalShadowDepthShader.h"
#include "../Output/Output.h"

bool LightComponent::shadowShaderInitialized = false;

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

void LightComponent::Initialize(Microsoft::WRL::ComPtr<ID3D12Device5> device)
{
	this->shadowViewport.TopLeftX = 0.0f;
	this->shadowViewport.TopLeftY = 0.0f;
	this->shadowViewport.Width = (float)this->shadowWidth;
	this->shadowViewport.Height = (float)this->shadowHeight;

	this->shadowViewport.MinDepth = 0.0f;
	this->shadowViewport.MaxDepth = 1.0f;

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
	this->shadowDepth.SetDepthBufferToSRVHandle(device, this->lightDescriptorHeap.GetCPUHandle(Slot::shadowDepthSlot));
	this->shadowDepth.SetDepthBufferToDSVHandle(device, this->shadowHeap.GetCPUHandle(0));

	if (!this->shadowShaderInitialized)
	{
		DirectionalShadowDepthShader::GetInstance()->Initialize(device, 0, nullptr, this->shadowDepth.GetRenderTargetDepthFormat(), 1);
		shadowShaderInitialized = true;
	}
	this->shadowDepthShader = DirectionalShadowDepthShader::GetInstance();
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
	commandList->SetGraphicsRootDescriptorTable(DirectionalLightShader::Slot::shadowDepth, this->lightDescriptorHeap.GetGPUHandle(Slot::shadowDepthSlot));
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
}

void LightComponent::SetFramebufferToNormalHandle(Microsoft::WRL::ComPtr<ID3D12Device5> device, RenderFramebuffer normalFramebuffer)
{
	normalFramebuffer.SetFramebufferToSRVHandle(device, this->lightDescriptorHeap.GetCPUHandle(Slot::normal));
}

void LightComponent::SetFramebufferToAlbedoSpecular(Microsoft::WRL::ComPtr<ID3D12Device5> device, RenderFramebuffer albedoSpecFramebuffer)
{
	albedoSpecFramebuffer.SetFramebufferToSRVHandle(device, this->lightDescriptorHeap.GetCPUHandle(Slot::albedoSpecular));
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

void LightComponent::RenderSceneToShadow(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList, 
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
