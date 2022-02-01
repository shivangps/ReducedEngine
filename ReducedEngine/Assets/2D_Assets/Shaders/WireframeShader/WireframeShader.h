#pragma once
#include "../../../../Output/Graphics/Shader.h"

// Class that handles the wireframe shader.

class WireframeShader : public Shader
{
private:
	WireframeShader() {}

public:
	enum CBSR_Slot
	{
		matrixDataSlot = 0,
		localPixelDataSlot,
	};

	// Get the single instance of the shader.
	static WireframeShader* GetInstance()
	{
		static WireframeShader* instance = new WireframeShader();
		return instance;
	}

	// Function to get the single instance of the object.
	void Initialize(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int numRT, DXGI_FORMAT renderTargetFormats[], DXGI_FORMAT depthStencilFormat, unsigned int samples);
};