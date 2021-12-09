#pragma once
#include "../Output/Graphics/Shader.h"

// Shader to post process the shadow and removing artifacts.

class ShadowPostProcessShader : public Shader
{
private:
	ShadowPostProcessShader() {}

	DXGI_FORMAT format = DXGI_FORMAT_R32_FLOAT;

public:
	static ShadowPostProcessShader* GetInstance()
	{
		static ShadowPostProcessShader* instance = new ShadowPostProcessShader();
		return instance;
	}

	void Initialize(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int numRT, DXGI_FORMAT renderTargetFormats[], unsigned int samples);
};