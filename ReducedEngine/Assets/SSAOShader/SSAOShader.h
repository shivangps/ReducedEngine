#pragma once
#include "../../Output/Graphics/Shader.h"

// Header file that contains the class decelration for SSAO shader.

class SSAOShader : public Shader
{
private:
	SSAOShader() {}

public:
	// Function to get the single instance of class.
	static SSAOShader* GetInstance()
	{
		static SSAOShader* instance = new SSAOShader();
		return instance;
	}

	// Render target format of the framebuffer.
	const DXGI_FORMAT ssaoRenderTargetFormat = DXGI_FORMAT_R32_FLOAT;

	// Function to initialize the shader.
	void Initialize(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int numRT, DXGI_FORMAT renderTargetFormats[], DXGI_FORMAT depthStencilFormat, unsigned int samples);
	void Initialize(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int numRT, DXGI_FORMAT renderTargetFormats[], unsigned int samples);
};