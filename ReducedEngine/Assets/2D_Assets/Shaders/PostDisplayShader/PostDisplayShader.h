#pragma once
#include "../Output/Graphics/Shader.h"

// Shader used in 2D graphics engine to display framebuffer results.

class PostDisplayShader : public Shader
{
private:
	PostDisplayShader() {}

public:
	static PostDisplayShader* GetInstance()
	{
		static PostDisplayShader* instance = new PostDisplayShader();
		return instance;
	}

	// Function to initalize the shader.
	void Initialize(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int numRT, DXGI_FORMAT renderTargetFormats[], DXGI_FORMAT depthStencilFormat, unsigned int samples);
};