#pragma once
#include "GUIComponent.h"

// This header contains the class definition for the collection of all the gui components to render in the scene.

struct GUIComponentInfo
{
	Boolean enable = Boolean(false, nullptr);
	UINT64 guiComponentIndex;
	GUIComponent* guiComponent = nullptr;
};

class GUIComponentList
{
private:
	bool stateEnableChange = false;
	std::vector<GUIComponentInfo> guiComponentList = {};
	UINT64 currentGUIComponentIndex = 0;

public:

	// Function to assign a GUI component to the render list.
	UINT64 AssignGUIComponent(GUIComponent* guiComponent);

	// Function to initialize all the GUI Components.
	void InitializeAllGUIComponents(Microsoft::WRL::ComPtr<ID3D12Device5> device, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList);
	// Function to update and render all the GUI components.
	void UpdateAllGUIComponents(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList, Matrix4 projection, unsigned int width, unsigned int height);
};