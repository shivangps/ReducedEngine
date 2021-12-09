#pragma once
#include "../Output/Graphics/Shader.h"

// Shader to handle the shadow rendering on a frambuffer for directional light.


class DirectionalShadowShader : public Shader
{
private:
	DirectionalShadowShader() {}

	DXGI_FORMAT format = DXGI_FORMAT_R32_FLOAT;

public:
	// Singleton.
	static DirectionalShadowShader* GetInstance()
	{
		static DirectionalShadowShader* instance = new DirectionalShadowShader();
		return instance;
	}

	void Initialize(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int numRT, DXGI_FORMAT renderTargetFormats[], unsigned int samples);
};