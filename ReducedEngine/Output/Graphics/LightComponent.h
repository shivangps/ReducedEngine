#pragma once
#include "../MathEngine.h"
#include "GraphicsHelper.h"
#include "../Assets/DirectionalLightShader/DirectionalLightShader.h"
#include "../Assets/DirectionalShadowShader/DirectionalShadowShader.h"
#include "../Assets/ShadowPostProcessShader/ShadowPostProcessShader.h"
#include "RenderFramebuffer.h"
#include "DepthFramebuffer.h"
#include "RenderList.h"
#include "Camera.h"
#include "Quad/Quad.h"

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
		shadowRenderSlot,
		ssao,
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

	Quad* quad = Quad::GetInstance();

	// Function to build the constant buffer for the light characteristics.
	void BuildLightConstantBuffer(Microsoft::WRL::ComPtr<ID3D12Device5> device);

public:
	LightComponent(Transform* transform) : transform(transform) {}

	// Function to get the transform component.
	Transform GetTransform();

	// Function to initialize the light component.
	virtual void Initialize(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int width, unsigned int height);

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
	// Function to set the framebuffer to ssao handle.
	void SetFramebufferToSSAO(Microsoft::WRL::ComPtr<ID3D12Device5> device, RenderFramebuffer ssaoFramenbuffer);

public:
	// Function to set the ambient light value.
	void SetAmbientColor(Vector3 ambient);
	// Function to set the diffuse light value.
	void SetDiffuseColor(Vector3 diffuse);
	// Function to set the specular light value.
	void SetSpecularColor(Vector3 specular);

private:
	// Shadows for directional light.

	unsigned int shadowHeight = 700;
	unsigned int shadowWidth = 700;
	D3D12_VIEWPORT shadowViewport = {};
	D3D12_RECT shadowClippingRect = {};

	DepthFramebuffer shadowDepth = {};

	DescriptorHeap shadowHeap = {};

	static bool shadowShaderInitialized;

	Shader* shadowDepthShader = nullptr;

	Matrix4 lightSpaceMatrix = {};

public:

	// Function to render all the render components in the scene for the shadow cast.
	void RenderSceneToShadowDepth(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList,
		unsigned int numberOfRenderTargets,
		D3D12_CPU_DESCRIPTOR_HANDLE* pRenderTargetHandle,
		bool RTsSingleHandleToDescriptorRange,
		D3D12_CPU_DESCRIPTOR_HANDLE* pDepthStencilTargetHandle,
		RenderList* renderComponentList, Vector3 shadowPosition,
		D3D12_VIEWPORT* currentSetViewport,
		D3D12_RECT* currentSetClipRect);

private:
	// Main Shadow Render.

	static bool initializedDirectionalShadowShader;
	Shader* shadowRenderShader = DirectionalShadowShader::GetInstance();

	enum SHADOW_RENDER_SLOT
	{
		cbv = 0,
		fragmentPosition_srv,
		shadowDepth_srv,
		normal_srv,
		size,
	};

	struct ShadowRenderCbv
	{
		unsigned int shadowWidth = 0;
		unsigned int shadowHeight = 0;
		int Pad0 = 0;
		int Pad1 = 0;
		Matrix4 lightSpaceMatrix;
	}shadowRenderDataCbv;
	Microsoft::WRL::ComPtr<ID3D12Resource> shadowRenderConstantBuffer = nullptr;
	UINT8* pShadowRenderData = nullptr;

	DescriptorHeap shadowRenderSrvHeap = {};
	DescriptorHeap shadowRenderRtvHeap = {};
	RenderFramebuffer shadowRenderBuffer = {};
	DXGI_FORMAT shadowRenderFormat = DXGI_FORMAT_R32_FLOAT;
	float shadowClearColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

private:
	// Shadow Post Process
	static bool initializedShadowPostProcessShader;
	Shader* postProcessShader = ShadowPostProcessShader::GetInstance();

	enum SHADOW_PP_SLOT
	{
		ppinputTexture_srv = 0,
		ppCbv,
		ppSize,
	};

	struct ShadowPostProcessData
	{
		unsigned int width = 0;
		unsigned int height = 0;
		int Pad0 = 0;
		int Pad1 = 0;
	}shadowPPData;
	Microsoft::WRL::ComPtr<ID3D12Resource> shadowPPConstantBuffer = nullptr;
	UINT8* pShadowPPCBData = nullptr;

	DescriptorHeap shadowPPSrvHeap = {};
	DescriptorHeap shadowPPRtvHeap = {};
	RenderFramebuffer shadowPPRenderBuffer = {};
	DXGI_FORMAT shadowPPFormat = DXGI_FORMAT_R32_FLOAT;
	float shadowPPClearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };


	void InitializeShadowRender(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int width, unsigned int height);

public:
	// Function to render the end result of shadow to another framebuffer adn post-process before sending it for lighting calculation.
	void RenderShadow(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList);

};