#pragma once
#include "../Output/Graphics/Shader.h"

// This header file contains the class definition of a cubemap shader.

class CubemapShader : public Shader
{
private:
	CubemapShader() {}

public:

	// Function to get single instance of class.
	static CubemapShader* GetInstance()
	{
		static CubemapShader* instance = new CubemapShader();
		return instance;
	}

	// Override of virtual function to initialize.
	void Initialize(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int numRT, DXGI_FORMAT renderTargetFormats[], DXGI_FORMAT depthStencilFormat, unsigned int samples);
};