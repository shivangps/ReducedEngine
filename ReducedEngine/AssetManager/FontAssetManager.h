#pragma once
#include "FontType.h"

// Class to maintain a single instance of a font type based on the name of the font.
class FontAssetManager
{
private:
	FontAssetManager() {}

	Microsoft::WRL::ComPtr<ID3D12Device5> graphicsDevice = nullptr;

	struct FontAsset
	{
		UINT64 fontIndex = 0;
		FontType fontType = {};
		std::string fileLocation = "";
		unsigned int fontSize = 10;
	};

	std::vector<FontAsset*> fontCollection = {};

	UINT64 currentFontIndex = 0;

public:
	// Get a single instance of this class.
	static FontAssetManager* GetInstance()
	{
		static FontAssetManager* instance = new FontAssetManager();
		return instance;
	}

	// Function to call for loading all the font texture data to CPU RAM.
	void Initialize(Microsoft::WRL::ComPtr<ID3D12Device5> device, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList);

	// Function to get the index for font.
	FontType* GetFont(UINT64 fontIndex);
	// Function to assign a new font as well as return the index.
	UINT64 SetNewFont(std::string fileLocation, unsigned int fontSize);
};