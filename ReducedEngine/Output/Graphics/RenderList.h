#pragma once
#include "RenderComponent.h"
#include "2D/RenderComponent2D.h"

// This header file contains the class definition of render compoenent collection class that is to be used for rendering.

struct RenderComponentInfo
{
	Boolean enable = Boolean(false, nullptr);
	UINT64 componentIndex = 0;
	RenderComponent* renderComponent = nullptr;
};

struct RenderComponent2DInfo
{
	Boolean enable = Boolean(false, nullptr);
	UINT64 componentIndex = 0;
	RenderComponent2D* renderComponent2d = nullptr;
};

class RenderList
{
private:
	bool stateEnableChange = false;
	bool stateEnableChange2D = false;
	std::vector<RenderComponentInfo> renderComponentList = {};
	std::vector<RenderComponent2DInfo> renderComponent2DList = {};
	UINT64 currentRenderComponentIndex = 0;
	UINT64 currentRenderComponent2DIndex = 0;

	// Background color of the scene.
	Vector3 bgColor = Vector3(0.5f);

	// Function to swap the contents of an element in the list in two poistions.
	void SwapContents(unsigned int firstPosition, unsigned int secondPosition);

public:

	// Function to assign a render component.
	UINT64 AssignRenderComponent(RenderComponent* renderComponent);
	UINT64 AssignRenderComponent2D(RenderComponent2D* renderComponent);
	// Function to call for initializing all the render compoenents assigned in the list.
	void InitializeComponents(Microsoft::WRL::ComPtr<ID3D12Device5> device, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList);
	void InitializeComponents2D(Microsoft::WRL::ComPtr<ID3D12Device5> device);
	// Function to render all the render component.
	void DrawAllComponents(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList, Camera camera);
	// Function to draw all render components using multi-threading.
	void DrawAllComponents(std::vector<ID3D12GraphicsCommandList4*> commandList, Camera camera);
	// Function to call the 2D render components.
	void DrawAll2DComponents(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList, Camera2D camera);
	// Function to render all the render components for shadow depth mapping.
	void DrawAllComponentsForShadow(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList, Matrix4 lightSpaceMatrix);
	// Function to initilize the command list bundles for all the render components.
	void InitializeAllBundleLists();
	// Function to set the bgColor of the scene.
	void SetBackgroundColor(Vector3 bgColor);
	// Function to get the background color.
	Vector3 GetBackgroundColor();
};