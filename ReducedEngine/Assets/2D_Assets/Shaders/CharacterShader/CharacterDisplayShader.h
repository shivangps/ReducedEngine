#pragma once
#include <DirectXMath.h>
#include "../Output/Graphics/Shader.h"

// This header contains the shader to simply display the texture.

class CharacterDisplayShader : public Shader
{
	CharacterDisplayShader() {}

public:

	// Function to get single instance of class.
	static CharacterDisplayShader* GetInstance()
	{
		static CharacterDisplayShader* instance = new CharacterDisplayShader();
		return instance;
	}

	// Override of virtual function to initialize.
	void Initialize(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int numRT, DXGI_FORMAT renderTargetFormats[], DXGI_FORMAT depthStencilFormat, unsigned int samples);
};