#pragma once
#include "GUIComponent.h"
#include "../AssetManager/TextureAssetManager.h"
#include "../Assets/2D_Assets/Shaders/CharacterShader/CharacterDisplayShader.h"
#include "../UniversalDescriptorHeap.h"
#include "../AssetManager/FontAssetManager.h"

// A GUI component that is responsible for rendering the text on the screen.

class TextGUIComponent : public GUIComponent
{
private:
	Microsoft::WRL::ComPtr<ID3D12Device5> graphicsDevice = nullptr;

	std::string text = "Sample";
	bool toCalculateText = false;
	std::string fontFileLocation = "";
	unsigned int fontSize = 150;
	float textSize = 1.0f;

	CharacterDisplayShader* shader = CharacterDisplayShader::GetInstance();

	TextureAssetManager* textureAssetManager = TextureAssetManager::GetInstance();

	Microsoft::WRL::ComPtr<ID3D12Resource> mainTexture = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> uploadTexture = nullptr;

	FontType* fontType;

	D3D12_GPU_DESCRIPTOR_HANDLE guiTransformationHandle = {};

	struct CharacterElement
	{
		char setCharacter = 'R';
		D3D12_GPU_DESCRIPTOR_HANDLE characterTextureHandle = {};
		D3D12_GPU_DESCRIPTOR_HANDLE characterTransformationHandle = {};
		UINT64 constantBufferDescriptorIndex = 0;
		GUITransformationData characterTransformationData = {};
		ConstantBuffer characterTransformConstantBuffer = {};
	};

	std::vector<CharacterElement> characterTransformElements;

	struct ColorConstantData
	{
		Vector3 color = Vector3(1.0f, 1.0f, 1.0f);
		int Pad0 = 0;
	}currentColorData;
	ConstantBuffer colorConstantBuffer = {};
	D3D12_GPU_DESCRIPTOR_HANDLE colorCbvHandle = {};

	// Function to recalculate text.
	void InitializeTextConstantBuffers();
	// Function to create a constant buffer for the color.
	void InitlializeColorConstantBuffer();

public:
	// REMOVE
	bool enableText = true;

	// Function to set the text string to be displayed.
	void AssignText(std::string text);
	// Function to set a new font to the text.
	void SetFont(std::string fileLocation, unsigned int fontSize);
	// Function to set he color for the text.
	void SetColor(Vector3 color);
	// Function to set the size of the text.
	void SetSize(float size);

public:
	TextGUIComponent(Transform2D* transform) : GUIComponent(transform) {}

	// Function to initialize the text rendering component.
	void InitializeGUI(Microsoft::WRL::ComPtr<ID3D12Device5> device, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList);
	// Function to call for rendering the text on the screen.
	void DrawGUI(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList, Matrix4 projection, unsigned int width, unsigned int height);
};