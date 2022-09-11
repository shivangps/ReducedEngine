#pragma once
#include "GUIComponent.h"
#include "../AssetManager/TextureAssetManager.h"


class ButtonGUIComponent : public GUIComponent
{
private:
	std::string textureFileLocation = DefaultTextureDirectory::WhiteTexture;

	class ButtonShader* buttonShader = nullptr;

	TextureAssetManager* textureAssetManager = TextureAssetManager::GetInstance();

	D3D12_GPU_DESCRIPTOR_HANDLE textureHandle = {};
	UINT64 textureIndex = 0;
	Texture* buttonTexture = nullptr;

	struct PixelData
	{
		Vector3 color;
		int Pad0 = 0;
	}pixelData;
	D3D12_GPU_DESCRIPTOR_HANDLE pixelDataHandle = {};
	ConstantBuffer pixelDataConstantBuffer = {};

	D3D12_GPU_DESCRIPTOR_HANDLE transformationDataHandle = {};

	// Function to update the transformation matrix.
	void UpdateTransformationData(Matrix4 projection, float width, float height);

public:
	ButtonGUIComponent(Transform2D* transform) : GUIComponent(transform) {}

	// Function to initialize the text rendering component.
	void InitializeGUI(Microsoft::WRL::ComPtr<ID3D12Device5> device, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList);
	// Function to call for rendering the text on the screen.
	void DrawGUI(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList, Matrix4 projection, unsigned int width, unsigned int height);
	// Function to assign the button texture.
	void AssignButtonTexture(std::string fileLocation);
	// Function to assign the color.
	void AssignButtonColor(Vector3 color);
};