#pragma once
#include "../GraphicsHelper.h"
#include "../RenderFramebuffer.h"
#include "../MathEngine.h"
#include "../Shader.h"
#include "../Assets/SSAOShader/SSAOShader.h"
#include "../Assets/SSAOBlurShader/SSAOBlurShader.h"
#include "../Camera.h"
#include "../Quad/Quad.h"

// Class to handle the post-processing effect of creating ambient occlusion in screen space.

static float lerp(float a, float b, float f)
{
	return a + f * (b - a);
}

class SSAO
{
private:
	SSAO() {}

	// AMBIENT OCCLUSION PART.
	// Shaders.
	Shader* ssaoShader = SSAOShader::GetInstance();

	// Enum for easy shader input mapping.
	enum SLOT
	{
		fragment_view_position_srv = 0,
		normal_srv,
		noise_srv,
		sample_description_cbv,
		sample_kernels_srv,
		size,
	};

	DescriptorHeap sourceHeap = {};
	DescriptorHeap rtvHeap = {};

	RenderFramebuffer mainFramebuffer = {};

	Microsoft::WRL::ComPtr<ID3D12Resource> kernelsDescResource = nullptr;
	unsigned char* pKernelsDescData = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> noiseTexture = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> noiseUploadTexture = nullptr;
	DXGI_FORMAT noiseTextureFormat = DXGI_FORMAT_R32G32B32_FLOAT;
	unsigned int noiseRows = 4;
	unsigned int noiseCols = 4;

	Microsoft::WRL::ComPtr<ID3D12Resource> kernelSamplesUploadBuffer = nullptr;

	DXGI_FORMAT ssaoFormat = DXGI_FORMAT_R32_FLOAT;
	float ssaoClearColor[4] = { 1.0f, 1.0f, 1.0f , 1.0f };

	const unsigned int numOfKernelSamples = 32;

	struct SSAOSampleDescription
	{
		unsigned int numOfSamples = 0;
		unsigned int screenWidth = 0;
		unsigned int screenHeight = 0;
		unsigned int noiseWidth = 0;
		unsigned int noiseHeight = 0;
		unsigned int Pad0 = 0;
		unsigned int Pad1 = 0;
		unsigned int Pad2 = 0;
		Matrix4 projection;
	}SampleDesc;

	struct SampleData
	{
		Vector3 samplePosition;
		int Pad0 = 0;
	};

private:
	// BLURRING PART.
	// Responsible for blurring the SSAO artifacts and make a smooth image.

	Shader* blurShader = SSAOBlurShader::GetInstance();

	DescriptorHeap blurSrvHeap = {};
	DescriptorHeap blurRtvHeap = {};

	RenderFramebuffer blurFramebuffer = {};

	float blurClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

	enum BLUR_SRV_SLOT
	{
		inputTexture = 0,
		constantBuffer,
		Size
	};

	struct BlurCB
	{
		unsigned int width = 0;
		unsigned int height = 0;
		int Pad0 = 0;
		int Pad1 = 0;
	}blur_cb;
	void InitializeBlurConstantBuffer(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int width, unsigned int height);
	Microsoft::WRL::ComPtr<ID3D12Resource> blurConstantBufferResource = nullptr;
	UINT8* pBlurCB = 0;

	// Quad geometry to render the post processed texture.
	Quad* quad = Quad::GetInstance();

public:
	// Functiont to get the single instance of the class.
	static SSAO* GetInstance()
	{
		static SSAO* instance = new SSAO();
		return instance;
	}

	// Function to initialize this class.
	void Initialize(Microsoft::WRL::ComPtr<ID3D12Device5> device, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList, unsigned int width, unsigned int height);

	// Function to set the output texture heap handle.
	void ApplyOutputTextureHandle(Microsoft::WRL::ComPtr<ID3D12Device5> device, D3D12_CPU_DESCRIPTOR_HANDLE handle);
	// Function to get the render framebuffer to apply the handle to.
	RenderFramebuffer GetOutputFramebuffer();

	// Function to apply the input texture for fragment view position to the heap handle.
	void SetFragmentViewPositionFramebufferToHeap(Microsoft::WRL::ComPtr<ID3D12Device5> device,  RenderFramebuffer* fragmentViewPositionBuffer);
	// Function to apply the input texture for normal framebuffer to the heap handle.
	void SetNormalFramebufferToHeap(Microsoft::WRL::ComPtr<ID3D12Device5> device, RenderFramebuffer* normalBuffer);

	// Function to perform ambient occlusion on the drawn G-Buffers.
	void PostProcessAmbientOcclusion(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList, Camera camera);
};