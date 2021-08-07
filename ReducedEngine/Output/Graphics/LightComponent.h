#pragma once
#include "../MathEngine.h"
#include "GraphicsHelper.h"
#include "../Assets/DirectionalLightShader/DirectionalLightShader.h"
#include "RenderFramebuffer.h"
#include "DepthFramebuffer.h"
#include "RenderList.h"
#include "Camera.h"

// This header file contains the class implementation for light in the rendering environment.

class LightComponent
{
private:
	// To store the pointer of the transform of its corresponding gameobject.
	Transform* transform = nullptr;

	Shader* directionalLightShader = DirectionalLightShader::GetInstance();

	typedef enum Slot
	{
		directionalLightCharacteristics = 0,
		fragmentPosition,
		normal,
		albedoSpecular,
		shadowDepthSlot,
		Size
	}Slot;

	struct DirectionalLightCahracteristics
	{
		DirectX::XMFLOAT4 direction;

		DirectX::XMFLOAT4 ambient;
		DirectX::XMFLOAT4 diffuse;
		DirectX::XMFLOAT4 specular;

		DirectX::XMFLOAT4 viewDirection;

		Matrix4 lightSpaceMatrix;

		float shadowWidth;
		float shadowHeight;
	}DLCharacteristics = {};

	// Light characteristics constant buffer.
	DescriptorHeap lightDescriptorHeap = {};
	Microsoft::WRL::ComPtr<ID3D12Resource> DLCConstantBuffer = nullptr;
	unsigned char* pLightCharacteristicsCBV = nullptr;

	// Function to build the constant buffer for the light characteristics.
	void BuildLightConstantBuffer(Microsoft::WRL::ComPtr<ID3D12Device5> device);

public:
	LightComponent(Transform* transform) : transform(transform) {}

	// Function to get the transform component.
	Transform GetTransform();

	// Function to initialize the light component.
	virtual void Initialize(Microsoft::WRL::ComPtr<ID3D12Device5> device);

	// Function to set the light shader for calculating the directional light for all the objects in the scene.
	void SetLightForRendering(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList, Camera camera);
	// Function to update constant buffer data.
	void UpdateConstantBuffer(Camera camera);

	// Function to set the framebuffer to fragment position handle.
	void SetFramebufferToFragmentPositionHandle(Microsoft::WRL::ComPtr<ID3D12Device5> device, RenderFramebuffer fragmentPositionFramebuffer);
	// Function to set the framebuffer to normal handle.
	void SetFramebufferToNormalHandle(Microsoft::WRL::ComPtr<ID3D12Device5> device, RenderFramebuffer normalFramebuffer);
	// Function to set the framebuffer to albedo and specular handle.
	void SetFramebufferToAlbedoSpecular(Microsoft::WRL::ComPtr<ID3D12Device5> device, RenderFramebuffer albedoSpecFramebuffer);

public:
	// Function to set the ambient light value.
	void SetAmbientColor(Vector3 ambient);
	// Function to set the diffuse light value.
	void SetDiffuseColor(Vector3 diffuse);
	// Function to set the specular light value.
	void SetSpecularColor(Vector3 specular);

private:
	// Shadows for directional light.

	unsigned int shadowHeight = 1000;
	unsigned int shadowWidth = 1000;
	D3D12_VIEWPORT shadowViewport = {};
	D3D12_RECT shadowClippingRect = {};

	DepthFramebuffer shadowDepth = {};

	DescriptorHeap shadowHeap = {};

	static bool shadowShaderInitialized;

	Shader* shadowDepthShader = nullptr;

	Matrix4 lightSpaceMatrix = {};

public:

	// Function to render all the render components in the scene for the shadow cast.
	void RenderSceneToShadow(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList,
		unsigned int numberOfRenderTargets,
		D3D12_CPU_DESCRIPTOR_HANDLE* pRenderTargetHandle,
		bool RTsSingleHandleToDescriptorRange,
		D3D12_CPU_DESCRIPTOR_HANDLE* pDepthStencilTargetHandle,
		RenderList* renderComponentList, Vector3 shadowPosition,
		D3D12_VIEWPORT* currentSetViewport,
		D3D12_RECT* currentSetClipRect);
};