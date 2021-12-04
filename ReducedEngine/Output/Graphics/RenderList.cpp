#include "RenderList.h"

void RenderList::SwapContents(unsigned int firstPosition, unsigned int secondPosition)
{
	RenderComponentInfo tempInfo = this->renderComponentList[firstPosition];
	this->renderComponentList[firstPosition] = this->renderComponentList[secondPosition];
	this->renderComponentList[secondPosition] = tempInfo;

	// Assign the new render component enable boolean pointer location to the respective render components.
	this->renderComponentList[firstPosition].renderComponent->SetEnable(&this->renderComponentList[firstPosition].enable);
	this->renderComponentList[secondPosition].renderComponent->SetEnable(&this->renderComponentList[secondPosition].enable);
}

UINT64 RenderList::AssignRenderComponent(RenderComponent* renderComponent)
{
	this->renderComponentList.push_back(RenderComponentInfo());
	RenderComponentInfo* info = &this->renderComponentList[this->renderComponentList.size() - 1];

	info->enable = Boolean(true, &this->stateEnableChange);
	info->componentIndex = this->currentRenderComponentIndex++;
	info->renderComponent = renderComponent;

	renderComponent->SetEnable(&info->enable);

	return info->componentIndex;
}

void RenderList::InitializeComponents(Microsoft::WRL::ComPtr<ID3D12Device5> device, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList)
{
	for (unsigned int i = 0; i < this->renderComponentList.size(); i++)
	{
		RenderComponent* renderComponent = renderComponentList[i].renderComponent;

		renderComponent->InitializeComponent(device, commandList);
		renderComponent->InitializeShadowConstantBuffer(device);
	}

	this->stateEnableChange = false;
}

void RenderList::DrawAllComponents(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList, Camera camera)
{
	for (unsigned int i = 0; i < this->renderComponentList.size(); i++)
	{
		if (renderComponentList[i].enable.GetBool())
		{
			RenderComponent* renderComponent = this->renderComponentList[i].renderComponent;

			renderComponent->Draw(commandList, camera);
		}
	}
}

void RenderList::DrawAllComponentsForShadow(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList, Matrix4 lightSpaceMatrix)
{
	for (unsigned int i = 0; i < this->renderComponentList.size(); i++)
	{
		if (renderComponentList[i].enable.GetBool())
		{
			RenderComponent* renderComponent = this->renderComponentList[i].renderComponent;

			renderComponent->DrawForShadow(commandList, lightSpaceMatrix);
		}
	}
}
