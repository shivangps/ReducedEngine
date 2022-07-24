#include "FontAssetManager.h"
#include "../Helper.h"

void FontAssetManager::Initialize(Microsoft::WRL::ComPtr<ID3D12Device5> device, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList)
{
	for (unsigned int i = 0; i < this->fontCollection.size(); i++)
	{
		this->fontCollection[i]->fontType.IntializeFontCharacterSet(device, commandList);
	}
}

FontType* FontAssetManager::GetFont(UINT64 fontIndex)
{
	// Check if the font index is exceeding the maximum amount.
	if (fontIndex < this->currentFontIndex)
	{
		// Look for the font index and return the FontType object initialized.
		for (unsigned int i = 0; i < this->fontCollection.size(); i++)
		{
			if (this->fontCollection[i]->fontIndex == fontIndex)
			{
				return &this->fontCollection[i]->fontType;
			}
		}
		ExitWithMessage("ERROR: The font index might be removed for the one being requested. Font Index Requested: " + std::to_string(fontIndex));
	}
	else
	{
		ExitWithMessage("ERROR: Font index exceeds the maximum amount.");
	}

	return nullptr;
}

UINT64 FontAssetManager::SetNewFont(std::string fileLocation, unsigned int fontSize)
{
	// Check if the file with the same name exists and return the index.
	for (unsigned int i = 0; i < this->fontCollection.size(); i++)
	{
		if (std::strcmp(this->fontCollection[i]->fileLocation.c_str(), fileLocation.c_str()) == 0 && this->fontCollection[i]->fontSize == fontSize)
		{
			return this->fontCollection[i]->fontIndex;
		}
	}

	// If no such file exists in the collection then create new one.
	FontAsset* newFontAsset = new FontAsset();

	newFontAsset->fileLocation = fileLocation;
	newFontAsset->fontIndex = this->currentFontIndex++;
	newFontAsset->fontSize = fontSize;
	newFontAsset->fontType.Initialize(fileLocation, fontSize);

	this->fontCollection.push_back(newFontAsset);

	return newFontAsset->fontIndex;
}