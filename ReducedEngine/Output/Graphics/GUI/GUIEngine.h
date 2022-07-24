#pragma once
#include "../UniversalDescriptorHeap.h"
#include "../GraphicsHelper.h"
#include "../RenderFramebuffer.h"
#include "TextGUIComponent.h"
#include "GUIComponentList.h"
#include "../AssetManager/FontAssetManager.h"

// Class responsible for the handling of the GUI elements for the game.
class GUIEngine
{
private:
	GUIEngine() {}

	GUIDescriptorHeap* GUIDH = GUIDescriptorHeap::GetInstance();
	UniversalDescriptorHeap* UDH = UniversalDescriptorHeap::GetInstance();
	RenderFramebuffer GUIFramebuffer = {};
	D3D12_GPU_DESCRIPTOR_HANDLE GUIFramebufferGPUHandle = {};

	FontAssetManager* fontAssetManager = FontAssetManager::GetInstance();

	// GUI Framebuffer background color.
	float bgColor[4] = {0.0f, 0.0f, 0.0f, 0.0f};

	// Render target descriptor heap.
	DescriptorHeap renderTargetHeap = {};

	// Projection Matrix.
	Matrix4 projection;

	// Screen width and height.
	unsigned int width = 500;
	unsigned int height = 500;

public:
	// Get a single instance of the class.
	static GUIEngine* GetInstance()
	{
		static GUIEngine* instance = new GUIEngine();
		return instance;
	}

	// Function to get the UI GPU shader resource handle in universal descriptor heap.
	D3D12_GPU_DESCRIPTOR_HANDLE GetUISrvGpuHandle();

	// FUnction to initialize the gui component list.
	void InitializeGUICompnentList(GUIComponentList* guiComponentList, Microsoft::WRL::ComPtr<ID3D12Device5> device, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList);
	 
	// Function to initalize the GUI engine.
	void Initalize(Microsoft::WRL::ComPtr<ID3D12Device5> device, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList, unsigned int width, unsigned int height);
	// Function to render the GUI elements.
	void RenderGUI(GUIComponentList* guiComponentList, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList, unsigned int numRT, D3D12_CPU_DESCRIPTOR_HANDLE* newRTHandle, D3D12_CPU_DESCRIPTOR_HANDLE* newDepthHandle);
};