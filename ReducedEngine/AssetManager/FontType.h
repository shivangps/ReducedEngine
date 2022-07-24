#pragma once
#include "../Output/Graphics/GraphicsHelper.h"
#include "../Output/Graphics/UniversalDescriptorHeap.h"
#include "../MathEngine.h"
#include <string>
#include <ft2build.h>
#include FT_FREETYPE_H
#define MAX_CHARACTER_SET 128

// Class that maintains all the collection of texture bitmaps of the characters of a particular font.
class FontType
{
private:
	// Contains all the characteristics of all the characters in the font.
	struct FontCharacter
	{
		Microsoft::WRL::ComPtr<ID3D12Resource> mainCharacterTexture = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource> uploadCharacterTexture = nullptr;
		D3D12_GPU_DESCRIPTOR_HANDLE udhHandle = {};
		Vector2 Size = Vector2();
		Vector2 Bearing = Vector2();
		unsigned int advanced = 0;
		bool invalid = true;
	};

	std::string fontFileLocation = "";
	unsigned int fontSize = 24;

	UniversalDescriptorHeap* universalDeascriptorHeap = UniversalDescriptorHeap::GetInstance();
	FontCharacter CharacterSet[MAX_CHARACTER_SET];

	bool IndexSanityCheck(unsigned int index);
public:
	void Initialize(std::string fontFileLocation, unsigned int fontSize);
	void IntializeFontCharacterSet(Microsoft::WRL::ComPtr<ID3D12Device5> device, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList);

	unsigned int GetAdvanceForCharacter(char character);
	Vector2 GetSizeOfCharacter(char character);
	Vector2 GetBearingOfCharacter(char character);
	D3D12_GPU_DESCRIPTOR_HANDLE GetCharacterHandle(char character);
};