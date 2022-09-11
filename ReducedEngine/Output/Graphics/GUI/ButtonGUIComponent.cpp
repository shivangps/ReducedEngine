#include "ButtonGUIComponent.h"
#include "../Output/Graphics/Quad/Quad.h"
#include "../Assets/2D_Assets/Shaders/ButtonShader/ButtonShader.h"

void ButtonGUIComponent::UpdateTransformationData(Matrix4 projection, float width, float height)
{
	Transform2D objectTransform = *this->transform;
	Matrix4 model = Matrix4();
	model = model.Scale(objectTransform.GetGlobalScale() * width / 2);
	model = model.Rotation(objectTransform.GetGlobalRotation());
	model = model.Translation(objectTransform.GetGlobalPostion());
	this->transformationData.TransformationMatrix = model.Transpose();

	this->transformationData.TransformationMatrix = projection * model;
	this->transformationConstantBuffer.CopyDataToConstantBufferLocation(&this->transformationData);
}

void ButtonGUIComponent::InitializeGUI(Microsoft::WRL::ComPtr<ID3D12Device5> device, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList)
{
	// Initialize the shader.
	this->buttonShader = ButtonShader::GetInstance();

	GUIDescriptorHeap* guiDescriptor = GUIDescriptorHeap::GetInstance();

	// Initialize the texture of the button.
	this->textureIndex = textureAssetManager->SetNewTexture(this->textureFileLocation);
	this->buttonTexture = textureAssetManager->GetTexture(this->textureIndex);
	this->textureHandle = guiDescriptor->GetCbvSrvUavGPUHandle(guiDescriptor->SetCpuHandle(device, this->buttonTexture->GetResource(), this->buttonTexture->GetShaderResourceView()));

	// Initialize the transformation data.
	std::string transformationConstantBufferName = (typeid(*this).name());
	transformationConstantBufferName = transformationConstantBufferName + " Transformation Constant Buffer";
	this->transformationConstantBuffer.Initialize(device, sizeof(this->transformationData), StringToWide(transformationConstantBufferName).c_str());
	this->transformationDataHandle = guiDescriptor->GetCbvSrvUavGPUHandle(guiDescriptor->SetCpuHandle(device, this->transformationConstantBuffer.GetConstantBufferViewDesc()));
	this->transformationData.TransformationMatrix = Matrix4();
	this->transformationConstantBuffer.CopyDataToConstantBufferLocation(&this->transformationData);

	// Initialize the pixel data.
	std::string pixelDataConstantBufferName = (typeid(*this).name());
	pixelDataConstantBufferName = pixelDataConstantBufferName + " Pixel Data Constant Buffer";
	this->pixelDataConstantBuffer.Initialize(device, sizeof(this->pixelData), StringToWide(pixelDataConstantBufferName).c_str());
	this->pixelDataHandle = guiDescriptor->GetCbvSrvUavGPUHandle(guiDescriptor->SetCpuHandle(device, this->transformationConstantBuffer.GetConstantBufferViewDesc()));
	this->pixelData.color = Vector3();
	this->pixelDataConstantBuffer.CopyDataToConstantBufferLocation(&this->pixelData);
}

void ButtonGUIComponent::DrawGUI(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList, Matrix4 projection, unsigned int width, unsigned int height)
{
	this->buttonShader->SetShaderForRender(commandList);

	this->UpdateTransformationData(projection, width, height);

	commandList->SetGraphicsRootDescriptorTable(ButtonShader::CBSR_Slot::textureSlot, this->textureHandle);
	commandList->SetGraphicsRootDescriptorTable(ButtonShader::CBSR_Slot::matrixDataSlot, this->transformationDataHandle);
	commandList->SetGraphicsRootDescriptorTable(ButtonShader::CBSR_Slot::localPixelDataSlot, this->pixelDataHandle);

	Quad::GetInstance()->Draw(commandList);
}

void ButtonGUIComponent::AssignButtonTexture(std::string fileLocation)
{
	this->textureFileLocation = fileLocation;
}

void ButtonGUIComponent::AssignButtonColor(Vector3 color)
{
	this->pixelData.color = color;
	this->pixelDataConstantBuffer.CopyDataToConstantBufferLocation(&this->pixelData);
}
