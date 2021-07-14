#pragma once
#include "../Output/Graphics/Shader.h"

// This header file contains the shader for directional light.

class DirectionalLightShader : public Shader
{
private:
	DirectionalLightShader() {}

public:
	typedef enum Slot
	{
		directionalLightCharacteristics = 0,
		fragmentPosition,
		normal,
		albedoSpecular,
		shadowDepth,
		Size
	}Slot;

	// Function to get the single instance of the class.
	static DirectionalLightShader* GetInstance()
	{
		static DirectionalLightShader* instance = new DirectionalLightShader();
		return instance;
	}

	// Function to initialize the shader.
	void Initialize(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int numRT, DXGI_FORMAT renderTargetFormats[], DXGI_FORMAT depthStencilFormat, unsigned int samples);
};