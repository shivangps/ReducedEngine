#include "FontType.h"
#include "../Output/Graphics/GUI/GUIComponent.h"
#include "../ErrorLogger.h"

bool FontType::IndexSanityCheck(unsigned int index)
{
	return index < MAX_CHARACTER_SET;
}

void FontType::Initialize(std::string fontFileLocation, unsigned int fontSize)
{
	this->fontFileLocation = fontFileLocation;
	this->fontSize = fontSize;
}

void FontType::IntializeFontCharacterSet(Microsoft::WRL::ComPtr<ID3D12Device5> device, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList)
{
	// FreeType state.
	FT_Library ft;
	FT_Face face;

	// Initialize the FT_Library.
	if (FT_Init_FreeType(&ft))
	{
		ExitWithMessage("ERROR: FREETYPE: Could init Free Type Library.");
	}

	// Initialize the FreeType face, which is basically font in freetype library language.
	if (FT_New_Face(ft, fontFileLocation.c_str(), 0, &face))
	{
		ExitWithMessage("ERROR: FREETYPE: Failed to load font.");
	}

	// Define the font size for rendering, this outputs the character texture resolution.
	FT_Set_Pixel_Sizes(face, 0, fontSize);

	// Describe and create a Texture2D.
	D3D12_RESOURCE_DESC textureDesc = {};
	textureDesc.MipLevels = 1;
	textureDesc.Format = DXGI_FORMAT_R8_UNORM;
	textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	textureDesc.DepthOrArraySize = 1;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

	// Get a single instance of the descriptor heap.
	GUIDescriptorHeap* guiDescriptorHeap = GUIDescriptorHeap::GetInstance();

	for (unsigned int i = 0; i < MAX_CHARACTER_SET; i++)
	{

		// Loading a particular character an generate its bitmap textrure.
		char currentCaracter = (char)i;
		if (FT_Load_Char(face, currentCaracter, FT_LOAD_RENDER))
		{
			ExitWithMessage("ERROR: FREETYPE: Failed to load Glyph of character: " + currentCaracter);
		}

		FontCharacter fontCharacter = FontCharacter();

		unsigned int CharacterWidth = face->glyph->bitmap.width;
		unsigned int CharacterHeight = face->glyph->bitmap.rows;
		unsigned int CharacterPixelSize = 1U;
		unsigned char* texture = face->glyph->bitmap.buffer;

		// Setting the characters size and bearing.
		fontCharacter.Size = Vector2(face->glyph->bitmap.width, face->glyph->bitmap.rows);
		fontCharacter.Bearing = Vector2(face->glyph->bitmap_left, face->glyph->bitmap_top);
		fontCharacter.advanced = face->glyph->advance.x;

		// If it is a space then give it a different characteristics.
		if ((char)i == ' ')
		{
			CharacterWidth = 1;
			CharacterHeight = 1;
			unsigned char unitTexture[1] = { 0x00 };
			texture = unitTexture;

			fontCharacter.Size = Vector2(1, 1);

			// Load character 'A' characteristics for fill for space but with a texture that has no pixels.
			FT_Face spaceReplacementFace = FT_Face();

			// Initialize the FreeType face, which is basically font in freetype library language.
			if (FT_New_Face(ft, fontFileLocation.c_str(), 0, &spaceReplacementFace))
			{
				ExitWithMessage("ERROR: FREETYPE: Failed to load font.");
			}

			// Define the font size for rendering, this outputs the character texture resolution.
			FT_Set_Pixel_Sizes(spaceReplacementFace, 0, fontSize);

			// For character 'A'.
			// Loading a particular character an generate its bitmap textrure.
			char currentCaracter = (char)'A';
			if (FT_Load_Char(spaceReplacementFace, currentCaracter, FT_LOAD_RENDER))
			{
				ExitWithMessage("ERROR: FREETYPE: Failed to load Glyph of character under the context of replacing ' ' space with 'A'");
			}

			fontCharacter.Bearing = Vector2(spaceReplacementFace->glyph->bitmap_left, spaceReplacementFace->glyph->bitmap_top);
			fontCharacter.advanced = spaceReplacementFace->glyph->advance.x;

			FT_Done_Face(spaceReplacementFace);
		}

		// Create texture for the current character font.
		HRESULT HR;
		textureDesc.Width = CharacterWidth;
		textureDesc.Height = CharacterHeight;

		HR = device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&textureDesc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(&fontCharacter.mainCharacterTexture));

		if (FAILED(HR))
		{
			fontCharacter.invalid = true;
			this->CharacterSet[i] = fontCharacter;
			continue;
		}

		const UINT64 uploadBufferSize = GetRequiredIntermediateSize(fontCharacter.mainCharacterTexture.Get(), 0, 1);
		std::string resourceName = (char)i + " Character Resource";
		fontCharacter.mainCharacterTexture->SetName(StringToWide(resourceName).c_str());

		// Create the GPU upload buffer.
		HR = device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&fontCharacter.uploadCharacterTexture));

		D3D12_SUBRESOURCE_DATA textureData = {};
		textureData.pData = texture;
		textureData.RowPitch = CharacterWidth * CharacterPixelSize;
		textureData.SlicePitch = textureData.RowPitch * CharacterHeight;

		UpdateSubresources(commandList.Get(), fontCharacter.mainCharacterTexture.Get(), fontCharacter.uploadCharacterTexture.Get(), 0, 0, 1, &textureData);
		commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(fontCharacter.mainCharacterTexture.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

		// Describe and create a SRV for the texture.
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = textureDesc.Format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
		UINT64 index = guiDescriptorHeap->SetCpuHandle(device, fontCharacter.mainCharacterTexture.Get(), &srvDesc);
		fontCharacter.udhHandle = guiDescriptorHeap->GetCbvSrvUavGPUHandle(index);

		if (currentCaracter == 'l')
		{
			fontCharacter.advanced = 1.25f * fontCharacter.advanced;
			//fontCharacter.Bearing.X(0.2f * fontCharacter.Bearing.X());
		}

		fontCharacter.invalid = false;

		this->CharacterSet[i] = fontCharacter;
	}

	FT_Done_Face(face);
	FT_Done_FreeType(ft);
}

unsigned int FontType::GetAdvanceForCharacter(char character)
{
	if (IndexSanityCheck((unsigned int)character))
	{
		if(!this->CharacterSet[(int)character].invalid)
			return this->CharacterSet[(int)character].advanced;
	}
	else
		ExitWithMessage("ERROR: Character index out of range for the GetAdvanceForCharacterFunction().");
}

Vector2 FontType::GetSizeOfCharacter(char character)
{
	if (IndexSanityCheck((unsigned int)character))
	{
		if(!this->CharacterSet[(int)character].invalid)
			return this->CharacterSet[(int)character].Size;
	}
	else
		ExitWithMessage("ERROR: Character index out of range for the GetSizeOfCharacter().");
}

Vector2 FontType::GetBearingOfCharacter(char character)
{
	if (IndexSanityCheck((unsigned int)character))
	{
		if (!this->CharacterSet[(int)character].invalid)
			return this->CharacterSet[(int)character].Bearing;
	}
	else
		ExitWithMessage("ERROR: Character index out of range for the GetBearingOfCharacter().");
}

D3D12_GPU_DESCRIPTOR_HANDLE FontType::GetCharacterHandle(char character)
{
	if (IndexSanityCheck((unsigned int)character))
	{
		if (!this->CharacterSet[(int)character].invalid)
			return this->CharacterSet[(int)character].udhHandle;
	}
	else
		ExitWithMessage("ERROR: Character index out of range for the GetCharacterHandle().");
}
