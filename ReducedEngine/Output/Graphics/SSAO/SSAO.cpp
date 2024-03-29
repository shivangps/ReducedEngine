#include "SSAO.h"
#include <random>

void SSAO::InitializeBlurConstantBuffer(Microsoft::WRL::ComPtr<ID3D12Device5> device, unsigned int width, unsigned int height)
{
	// Creata constant buffer for the blur shader.
	this->blur_cb.width = width;
	this->blur_cb.height = height;

	this->blurConstantBuffer.Initialize(device, sizeof(blur_cb), L"SSAO Blur Constant Buffer");

	this->blurConstantBufferHandle = this->universalDescHeap->GetCbvSrvUavGPUHandle(this->universalDescHeap->SetCpuHandle(device, this->blurConstantBuffer.GetConstantBufferViewDesc()));

	this->blurConstantBuffer.CopyDataToConstantBufferLocation(&this->blur_cb);
}

void SSAO::Initialize(Microsoft::WRL::ComPtr<ID3D12Device5> device, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList, unsigned int width, unsigned int height)
{
	// AMBIENT OCCLUSION.

	// Initialize the SSAO shader.
	DXGI_FORMAT ssaoFramebufferFormat[1] = { this->ssaoFormat };
	this->ssaoShader->Initialize(device, 1, ssaoFramebufferFormat, 1);

	// Initialize all the heaps.
	// Heap for render target.
	this->rtvHeap.Initialize(device, 1, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);

	// Initialize the framebuffer.
	this->mainFramebuffer.Initialize(device, this->ssaoFormat, width, height, 1, this->ssaoClearColor, L"SSAO");
	this->mainFramebuffer.SetFramebufferToRTVHandle(device, this->rtvHeap.GetCPUHandle(0));

	// Create Kernel samples for sampling the position and depth for ambient occlusion.
	std::uniform_real_distribution<float> randomFloats(0.0, 1.0);
	std::default_random_engine generator;
	std::vector<SampleData> ssaoKernel;
	for (unsigned int i = 0; i < this->numOfKernelSamples; i++)
	{
		Vector3 kernelPosition(randomFloats(generator) * 2.0f - 1.0f, randomFloats(generator) * 2.0f - 1.0f, randomFloats(generator));
		kernelPosition = kernelPosition.Normalize();
		kernelPosition *= randomFloats(generator);
		float scale = float(i) / float(this->numOfKernelSamples);

		// Scale samples s.t. they're more aligned to center of kernel.
		scale = lerp(0.1f, 1.0f, scale * scale);
		kernelPosition *= scale;

		SampleData data;
		data.samplePosition = kernelPosition;

		ssaoKernel.push_back(data);
	}

	// Create shader resource view for kernel samples to be used in shader.
	int Rows = 1;
	int Cols = ssaoKernel.size() * sizeof(SampleData);

	InitializeResource(&this->kernelSamplesUploadBuffer, device, &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		&CD3DX12_RESOURCE_DESC::Buffer(Rows * Cols), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr);

	this->kernelSamplesUploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&this->pKernelsDescData));
	this->kernelSamplesUploadBuffer->Unmap(0, nullptr);

	// Transfer data of samples to the resource buffer.
	memcpy(this->pKernelsDescData, &ssaoKernel[0], ssaoKernel.size() * sizeof(SampleData));

	// Noise texture data generation.
	UINT pixelSize = 3;
	UINT size = this->noiseRows * this->noiseCols * pixelSize;
	std::vector<float> ssaoNoise(size);

	for (unsigned int i = 0; i < size; i += pixelSize)
	{
		ssaoNoise[i] = randomFloats(generator) * 2.0f - 1.0f;
		ssaoNoise[i + 1] = randomFloats(generator) * 2.0f - 1.0f;
		ssaoNoise[i + 2] = randomFloats(generator);
	}

	// Create the noise texture.

	{
		// Describe and create the texture2D.
		D3D12_RESOURCE_DESC textureDesc = {};
		ZeroMemory(&textureDesc, sizeof(textureDesc));
		textureDesc.MipLevels = 1;
		textureDesc.Format = this->noiseTextureFormat;
		textureDesc.Width = this->noiseCols;
		textureDesc.Height = this->noiseRows;
		textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
		textureDesc.DepthOrArraySize = 1;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

		InitializeResource(&this->noiseTexture, device, &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), &textureDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr);

		const UINT64 uploadBufferSize = GetRequiredIntermediateSize(this->noiseTexture.Get(), 0, 1);

		InitializeResource(&noiseUploadTexture, device, &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), &CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr);

		D3D12_SUBRESOURCE_DATA textureData = {};
		textureData.pData = &ssaoNoise[0];
		textureData.RowPitch = this->noiseRows * pixelSize;
		textureData.SlicePitch = this->noiseCols * this->noiseRows * pixelSize;

		UpdateSubresources(commandList.Get(), this->noiseTexture.Get(), noiseUploadTexture.Get(), 0, 0, 1, &textureData);
		TransitionResourceState(this->noiseTexture, commandList, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

		// SRV creation.
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = this->noiseTextureFormat;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;

		this->noiseHandle = this->universalDescHeap->GetCbvSrvUavGPUHandle(this->universalDescHeap->SetCpuHandle(
			device, this->noiseTexture.Get(), &srvDesc));
	}

	// SSAO samples constant buffer.
	this->SampleDesc.numOfSamples = this->numOfKernelSamples;
	this->SampleDesc.screenWidth = width;
	this->SampleDesc.screenHeight = height;
	this->SampleDesc.noiseWidth = this->noiseCols;
	this->SampleDesc.noiseHeight = this->noiseRows;

	this->ssaoConstantBuffer.Initialize(device, sizeof(this->SampleDesc), L"SSAO Constant Buffer");
	this->sampleDescriptionHandle = this->universalDescHeap->GetCbvSrvUavGPUHandle(this->universalDescHeap->SetCpuHandle(
		device, this->ssaoConstantBuffer.GetConstantBufferViewDesc()));
	this->ssaoConstantBuffer.CopyDataToConstantBufferLocation(&this->SampleDesc);

	// BLUR.

	DXGI_FORMAT blurFramebufferFormat[] = { this->ssaoFormat };
	this->blurShader->Initialize(device, 1, blurFramebufferFormat, 1);

	this->blurRtvHeap.Initialize(device, 1, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
	this->blurFramebuffer.Initialize(device, this->ssaoFormat, width, height, 1, this->blurClearColor, L"SSAO Blur");

	this->blurInputTextureHandle = this->mainFramebuffer.GetGpuHandleForShaderResource();

	this->blurFramebuffer.SetFramebufferToRTVHandle(device, this->blurRtvHeap.GetCPUHandle(0));
	this->InitializeBlurConstantBuffer(device, width, height);

	// Assign the render framebuffer to the universal descriptor heap.
	this->ssaoHandle = this->universalDescHeap->GetCbvSrvUavGPUHandle(this->universalDescHeap->SetCpuHandle(
		device, this->blurFramebuffer.GetResourceTexture(), this->blurFramebuffer.GetShaderResourceView()));
}

void SSAO::ApplyOutputTextureHandle(Microsoft::WRL::ComPtr<ID3D12Device5> device, D3D12_CPU_DESCRIPTOR_HANDLE handle)
{
	this->blurFramebuffer.SetFramebufferToSRVHandle(device, handle);
}

RenderFramebuffer SSAO::GetOutputFramebuffer()
{
	return this->blurFramebuffer;
}

void SSAO::SetFragmentViewPositionFramebufferToHeap(Microsoft::WRL::ComPtr<ID3D12Device5> device, RenderFramebuffer* fragmentViewPositionBuffer)
{
	this->fragmentViewPositionHandle = fragmentViewPositionBuffer->GetGpuHandleForShaderResource();
}

void SSAO::SetNormalFramebufferToHeap(Microsoft::WRL::ComPtr<ID3D12Device5> device, RenderFramebuffer* normalBuffer)
{
	this->normalPositionHandle = normalBuffer->GetGpuHandleForShaderResource();
}

void SSAO::PostProcessAmbientOcclusion(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList, Camera camera)
{
	// AMBIENT OCCLUSION.

	// Update constant buffer data for ssao.
	this->SampleDesc.projection = camera.projection.Transpose();

	this->ssaoConstantBuffer.CopyDataToConstantBufferLocation(&this->SampleDesc);

	this->ssaoShader->SetShaderForRender(commandList);

	commandList->ClearRenderTargetView(this->rtvHeap.GetCPUHandle(0), this->ssaoClearColor, 0, nullptr);

	commandList->OMSetRenderTargets(1, &this->rtvHeap.GetCPUHandle(0), false, nullptr);

	commandList->SetGraphicsRootDescriptorTable(SLOT::fragment_view_position_srv, this->fragmentViewPositionHandle);
	commandList->SetGraphicsRootDescriptorTable(SLOT::normal_srv, this->normalPositionHandle);
	commandList->SetGraphicsRootDescriptorTable(SLOT::noise_srv, this->noiseHandle);
	commandList->SetGraphicsRootDescriptorTable(SLOT::sample_description_cbv, this->sampleDescriptionHandle);
	commandList->SetGraphicsRootShaderResourceView(SLOT::sample_kernels_srv, this->kernelSamplesUploadBuffer->GetGPUVirtualAddress());

	this->quad->Draw(commandList);

	this->mainFramebuffer.CopyResource(commandList);

	// BLUR

	this->blurShader->SetShaderForRender(commandList);

	commandList->ClearRenderTargetView(this->blurRtvHeap.GetCPUHandle(0), this->blurClearColor, 0, nullptr);

	commandList->OMSetRenderTargets(1, &this->blurRtvHeap.GetCPUHandle(0), false, nullptr);

	commandList->SetGraphicsRootDescriptorTable(BLUR_SRV_SLOT::inputTexture, this->blurInputTextureHandle);
	commandList->SetGraphicsRootDescriptorTable(BLUR_SRV_SLOT::constantBuffer, this->blurConstantBufferHandle);

	this->quad->Draw(commandList);

	this->blurFramebuffer.CopyResource(commandList);
}

D3D12_GPU_DESCRIPTOR_HANDLE* SSAO::GetGPUHandleForOutput()
{
	return &this->ssaoHandle;
}
