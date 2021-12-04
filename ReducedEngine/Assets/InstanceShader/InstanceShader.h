#pragma once
#include "../Output/Graphics/Shader.h"

// Header file that contains the shader for instancing unlit objects.

class InstanceShader : public Shader
{
	InstanceShader() {}

public:

	// Function to get single instance of class.
	static InstanceShader* GetInstance()
	{
		static InstanceShader* instance = new InstanceShader();
		return instance;
	}

	// Override the virtual function to initialize.
	void Initialize(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int numRT, DXGI_FORMAT renderTargetFormats[], DXGI_FORMAT depthStencilFormat, unsigned int samples);
};