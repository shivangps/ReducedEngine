#pragma once
#include"../../Output/Graphics/Shader.h"

class SSAOBlurShader : public Shader
{
private:
	SSAOBlurShader() {}

	DXGI_FORMAT pipelineFormat = DXGI_FORMAT_R32_FLOAT;

public:
	// Function to get single instance of class.
	static SSAOBlurShader* GetInstance()
	{
		static SSAOBlurShader* instance = new SSAOBlurShader();
		return instance;
	}

	// Override of virtual function to initialize.
	void Initialize(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int numRT, DXGI_FORMAT renderTargetFormats[], unsigned int samples);
};