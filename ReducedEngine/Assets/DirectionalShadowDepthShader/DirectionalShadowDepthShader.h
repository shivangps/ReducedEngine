#pragma once
#include "../Output/Graphics/Shader.h"

// This header file contains the single instance shader definition for shadow depth rendering.

class DirectionalShadowDepthShader : public Shader
{
private:
	DirectionalShadowDepthShader() {}

public:
	static DirectionalShadowDepthShader* GetInstance()
	{
		static DirectionalShadowDepthShader* instance = new DirectionalShadowDepthShader();
		return instance;
	}

	// Initialize the shadow depth shader.
	void Initialize(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int numRT, DXGI_FORMAT renderTargetFormats[], DXGI_FORMAT depthStencilFormat, unsigned int samples);
};