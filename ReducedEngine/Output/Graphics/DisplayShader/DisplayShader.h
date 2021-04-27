#pragma once
#include <DirectXMath.h>
#include "..//Shader.h"

// This header contains the shader to simply display the texture.

class DisplayShader : public Shader
{
	DisplayShader() {}

public:

	// Function to get single instance of class.
	static DisplayShader* GetInstance()
	{
		static DisplayShader* instance = new DisplayShader();
		return instance;
	}

	// Override of virtual function to initialize.
	void Initialize(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int numRT, DXGI_FORMAT renderTargetFormats[], DXGI_FORMAT depthStencilFormat, unsigned int samples);
};