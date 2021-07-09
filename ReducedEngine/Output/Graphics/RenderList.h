#pragma once
#include "RenderComponent.h"

// This header file contains the class definition of render compoenent collection class that is to be used for rendering.

class RenderList
{
private:
	struct RenderComponentInfo
	{
		UINT64 componentIndex = 0;
		RenderComponent* renderComponent = nullptr;
	};
	std::vector<RenderComponentInfo> renderComponentList = {};
	UINT64 currentRenderComponentIndex = 0;

public:

	// Function to assign a render component.
	UINT64 AssignRenderComponent(RenderComponent* renderComponent);
	// Function to call for initializing all the render compoenents assigned in the list.
	void InitializeComponents(Microsoft::WRL::ComPtr<ID3D12Device5> device, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList);
	// Function to render all the render component.
	void DrawAllComponents(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList, Camera camera);
};