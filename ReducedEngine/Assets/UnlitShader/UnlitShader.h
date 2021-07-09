#pragma once
#include <DirectXMath.h>
#include "..//..//Output/Graphics/Shader.h"

// This header contains the shader to simply render the mesh in plain color.

class UnlitShader : public Shader
{
	UnlitShader() {}

public:

	// Function to get single instance of class.
	static UnlitShader* GetInstance()
	{
		static UnlitShader* instance = new UnlitShader();
		return instance;
	}

	// Override the virtual function to initialize.
	void Initialize(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int numRT, DXGI_FORMAT renderTargetFormats[], DXGI_FORMAT depthStencilFormat, unsigned int samples);
};