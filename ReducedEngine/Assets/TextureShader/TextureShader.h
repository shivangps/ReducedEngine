#pragma once
#include <DirectXMath.h>
#include "..//..//Output/Graphics/Shader.h"

// This header contains the shader to simply render the mesh with texture color.

class TextureShader : public Shader
{
	TextureShader() {}

public:

	// Function to get single instance of a class.
	static TextureShader* GetInstance()
	{
		static TextureShader* instance = new TextureShader();
		return instance;
	}

	// Override the virtual function to initialize.
	void Initialize(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int numRT, DXGI_FORMAT renderTargetFormats[], DXGI_FORMAT depthStencilFormat, unsigned int samples);
};