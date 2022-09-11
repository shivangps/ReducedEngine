#pragma once
#include <DirectXMath.h>
#include "../Output/Graphics/Shader.h"

// This is a header that contains the class definition of alpha based texture shader.

class ButtonShader : public Shader
{
	ButtonShader() {}

public:
	enum CBSR_Slot
	{
		textureSlot = 0,
		matrixDataSlot,
		localPixelDataSlot,
	};

	// Function to get single instance of class.
	static ButtonShader* GetInstance()
	{
		static ButtonShader* instance = new ButtonShader();
		return instance;
	}

	// Override of virtual function to initialize.
	void Initialize(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int numRT, DXGI_FORMAT renderTargetFormats[], DXGI_FORMAT depthStencilFormat, unsigned int samples);
};