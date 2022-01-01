#pragma once
#include "RenderComponent.h"

// This header file contains the class definition of render compoenent collection class that is to be used for rendering.

struct RenderComponentInfo
{
	Boolean enable = Boolean(false, nullptr);
	UINT64 componentIndex = 0;
	RenderComponent* renderComponent = nullptr;
};

class RenderList
{
private:
	bool stateEnableChange = false;
	std::vector<RenderComponentInfo> renderComponentList = {};
	UINT64 currentRenderComponentIndex = 0;

	// Function to swap the contents of an element in the list in two poistions.
	void SwapContents(unsigned int firstPosition, unsigned int secondPosition);

public:

	// Function to assign a render component.
	UINT64 AssignRenderComponent(RenderComponent* renderComponent);
	// Function to call for initializing all the render compoenents assigned in the list.
	void InitializeComponents(Microsoft::WRL::ComPtr<ID3D12Device5> device, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList);
	// Function to render all the render component.
	void DrawAllComponents(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList, Camera camera);
	// Function to draw all render components using multi-threading.
	void DrawAllComponents(std::vector<ID3D12GraphicsCommandList4*> commandList, Camera camera);
	// Function to render all the render components for shadow depth mapping.
	void DrawAllComponentsForShadow(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList, Matrix4 lightSpaceMatrix);
	// Function to initilize the command list bundles for all the render components.
	void InitializeAllBundleLists();
};