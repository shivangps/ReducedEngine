#include "GUIEngine.h"

D3D12_GPU_DESCRIPTOR_HANDLE GUIEngine::GetUISrvGpuHandle()
{
	return this->GUIFramebufferGPUHandle;
}

void GUIEngine::InitializeGUICompnentList(GUIComponentList* guiComponentList, Microsoft::WRL::ComPtr<ID3D12Device5> device, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList)
{
	this->fontAssetManager->Initialize(device, commandList);

	guiComponentList->InitializeAllGUIComponents(device, commandList);
}

void GUIEngine::Initalize(Microsoft::WRL::ComPtr<ID3D12Device5> device, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList, unsigned int width, unsigned int height)
{
	// Initialize the gui descriptor heap.
	this->GUIDH->Initialize(device);

	// Initialize the width and height of the member variables.
	this->width = width;
	this->height = height;

	// Initialize the render target descriptor heap.
	this->renderTargetHeap.Initialize(device, 1, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);

	// Initialize the render framebuffer for the GUI to be rendered upon.
	this->GUIFramebuffer.Initialize(device, DXGI_FORMAT_R8G8B8A8_UNORM, width, height, 1, this->bgColor, L"GUI Render Framebuffer");
	this->GUIFramebuffer.SetFramebufferToRTVHandle(device, this->renderTargetHeap.GetCPUHandle(0));
	this->GUIFramebufferGPUHandle = this->UDH->GetCbvSrvUavGPUHandle(this->UDH->SetCpuHandle(device, this->GUIFramebuffer.GetResourceTexture(), this->GUIFramebuffer.GetShaderResourceView()));

	// Initialization of the projection matrix.
	this->projection = DirectX::XMMatrixOrthographicLH(width, height, 0.1f, 10.0f);
}

void GUIEngine::RenderGUI(GUIComponentList* guiComponentList, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList, unsigned int numRT, D3D12_CPU_DESCRIPTOR_HANDLE* newRTHandle, D3D12_CPU_DESCRIPTOR_HANDLE* newDepthHandle)
{
	// Clear the render target for the GUI elements to render.
	commandList->ClearRenderTargetView(this->renderTargetHeap.GetCPUHandle(0), this->bgColor, 0, nullptr);
	// Set the render framebuffer for render target to render the GUI.
	commandList->OMSetRenderTargets(1, &this->renderTargetHeap.GetCPUHandle(0), true, nullptr);

	this->GUIDH->AssignToGraphicsCommandList(commandList);

	// EXPERIMENTAL: TO BE REMOVED.
	//this->textGuiComponent.DrawGUI(commandList, this->projection, this->width, this->height);
	guiComponentList->UpdateAllGUIComponents(commandList, projection, width, height);

	this->UDH->AssignToGraphicsCommandList(commandList);

	// Remove the GUI render framebuffer. And copy the contents of the render target buffer to the shader resource texture of same size.
	commandList->OMSetRenderTargets(numRT, newRTHandle, true, newDepthHandle);
	this->GUIFramebuffer.CopyResource(commandList);
}
