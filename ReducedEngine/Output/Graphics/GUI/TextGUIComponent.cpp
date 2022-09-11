#include "TextGUIComponent.h"
#include "../ErrorLogger.h"
#include "../Quad/Quad.h"

void TextGUIComponent::AssignText(std::string text)
{
	this->text = text;
	this->toCalculateText = true;
}

void TextGUIComponent::InitializeTextConstantBuffers()
{
	if (this->toCalculateText)
	{
		GUIDescriptorHeap* guiDescriptorHeap = GUIDescriptorHeap::GetInstance();

	// Reset the constant buffer array.
	if (this->characterTransformElements.size() != 0)
	{
		for (unsigned int i = 0; i < this->characterTransformElements.size(); i++)
		{
			guiDescriptorHeap->ReleaseCbvSrvUavDescriptor(this->characterTransformElements[i].constantBufferDescriptorIndex);
		}
	}
	this->characterTransformElements.clear();

	// Process each character element.
	char currentCharacter = 'R';
	for (unsigned int currentCharacterIndex = 0; currentCharacterIndex < this->text.size(); currentCharacterIndex++)
	{
		// Get the current character from text to process.
		currentCharacter = this->text[currentCharacterIndex];

		this->characterTransformElements.push_back(CharacterElement());
		CharacterElement* characterElement = &this->characterTransformElements[this->characterTransformElements.size() - 1];

		// Set the character for the element.
		characterElement->setCharacter = currentCharacter;

		// Get the gpu descriptor handle for the character texture.
		characterElement->characterTextureHandle = this->fontType->GetCharacterHandle(currentCharacter);

		// Create a new transformation object for the character.
		characterElement->characterTransformationData = GUITransformationData();

		// Create a new constant buffer for the character transformation.
		std::string resourceString = "\"" + this->text + "\"" + " Constant Buffer";
		characterElement->characterTransformConstantBuffer.Initialize(this->graphicsDevice, sizeof(characterElement->characterTransformationData), StringToWide(resourceString).c_str());

		// Get the gpu descriptor handle fo the constant buffer.
		characterElement->constantBufferDescriptorIndex = guiDescriptorHeap->SetCpuHandle(this->graphicsDevice, characterElement->characterTransformConstantBuffer.GetConstantBufferViewDesc());
		characterElement->characterTransformationHandle = guiDescriptorHeap->GetCbvSrvUavGPUHandle(characterElement->constantBufferDescriptorIndex);

		// Assign the data to the constant buffer.
		characterElement->characterTransformConstantBuffer.CopyDataToConstantBufferLocation(&characterElement->characterTransformationData);
	}

	this->toCalculateText = false;
	}
}

void TextGUIComponent::InitlializeColorConstantBuffer()
{
	// Initilize the constant buffer for the character color.
	this->colorConstantBuffer.Initialize(this->graphicsDevice, sizeof(this->currentColorData), L"Character Color");
	
	// Assign the descriptor heap of the color constant buffer for shader.
	GUIDescriptorHeap* guiDescriptorHeap = GUIDescriptorHeap::GetInstance();
	this->colorCbvHandle = guiDescriptorHeap->GetCbvSrvUavGPUHandle(guiDescriptorHeap->SetCpuHandle(this->graphicsDevice, this->colorConstantBuffer.GetConstantBufferViewDesc()));

	// Copy the data constants to the constant buffer.
	this->colorConstantBuffer.CopyDataToConstantBufferLocation(&this->currentColorData);
}

void TextGUIComponent::SetFont(std::string fileLocation, unsigned int fontSize)
{
	FontAssetManager* fontAssetManager = FontAssetManager::GetInstance();
	this->fontType = fontAssetManager->GetFont(fontAssetManager->SetNewFont(fileLocation, fontSize));
}

void TextGUIComponent::SetColor(Vector3 color)
{
	this->currentColorData.color = color;
}

void TextGUIComponent::SetSize(float size)
{
	this->textSize = size;
}

void TextGUIComponent::InitializeGUI(Microsoft::WRL::ComPtr<ID3D12Device5> device, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList)
{
	this->graphicsDevice = device;

	// Initialize the transformation data constant buffer.
	this->transformationConstantBuffer.Initialize(device, sizeof(GUITransformationData), L"GUI Transformation");
	GUIDescriptorHeap* guiDescriptorHeap = GUIDescriptorHeap::GetInstance();
	this->guiTransformationHandle = guiDescriptorHeap->GetCbvSrvUavGPUHandle(guiDescriptorHeap->SetCpuHandle(device, this->transformationConstantBuffer.GetConstantBufferViewDesc()));

	this->InitializeTextConstantBuffers();
	this->InitlializeColorConstantBuffer();
}

void TextGUIComponent::DrawGUI(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList, Matrix4 projection, unsigned int width, unsigned int height)
{
	if (this->enableText)
	{
		this->shader->SetShaderForRender(commandList);
		FontType cachedFontType = *this->fontType;

		// Reassign color.
		this->colorConstantBuffer.CopyDataToConstantBufferLocation(&this->currentColorData);

		// Recalculate the text if it has been changed.
		this->InitializeTextConstantBuffers();

		// Calculate the width of the overall text.
		float spacing = this->textSize * 2.0f;
		float overallWidth = 0.0f;
		for (unsigned int currentElement = 0; currentElement < this->characterTransformElements.size(); currentElement++)
		{
			char character = this->characterTransformElements[currentElement].setCharacter;
			overallWidth = overallWidth + (spacing * float(cachedFontType.GetAdvanceForCharacter(character) >> 6));
		}

		// Display all the characters of the text.
		Vector2 origin = Vector2((-(float)overallWidth / 2.0f) + this->transform->GetGlobalPostion().X(), this->transform->GetGlobalPostion().Y());
		for (unsigned int currentElement = 0; currentElement < this->characterTransformElements.size(); currentElement++)
		{
			Transform2D spacingTransform = Transform2D();
			char character = this->characterTransformElements[currentElement].setCharacter;

			spacingTransform.SetRotation(this->transform->GetGlobalRotation());
			spacingTransform.SetScale(spacingTransform.GetGlobalScale() * Vector2(this->textSize * cachedFontType.GetSizeOfCharacter(character).X() / (float)width, this->textSize * cachedFontType.GetSizeOfCharacter(character).Y() / (float)height));
			float xPos = (origin.X() + this->textSize * (cachedFontType.GetBearingOfCharacter(character).X() + cachedFontType.GetSizeOfCharacter(character).X() / 2.0f)) / (float)width;
			float yPos = (origin.Y() - this->textSize * (cachedFontType.GetSizeOfCharacter(character).Y() - (cachedFontType.GetBearingOfCharacter(character).Y()) * 2.0f)) / (float)height;
			spacingTransform.SetPosition(xPos, yPos);
			this->characterTransformElements[currentElement].characterTransformationData.TransformationMatrix = (spacingTransform.GetGlobalModel()).Transpose();
			this->characterTransformElements[currentElement].characterTransformConstantBuffer.CopyDataToConstantBufferLocation(&this->characterTransformElements[currentElement].characterTransformationData);

			commandList->SetGraphicsRootDescriptorTable(0, this->characterTransformElements[currentElement].characterTextureHandle);
			commandList->SetGraphicsRootDescriptorTable(1, this->characterTransformElements[currentElement].characterTransformationHandle);
			commandList->SetGraphicsRootDescriptorTable(2, this->colorCbvHandle);

			Quad::GetInstance()->Draw(commandList);

			origin.X(origin.X() + (spacing * (float)(cachedFontType.GetAdvanceForCharacter(character) >> 6)));
		}
	}
}
