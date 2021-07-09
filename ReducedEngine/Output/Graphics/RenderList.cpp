#include "RenderList.h"

UINT64 RenderList::AssignRenderComponent(RenderComponent* renderComponent)
{
	RenderComponentInfo info;

	info.componentIndex = this->currentRenderComponentIndex++;
	info.renderComponent = renderComponent;

	this->renderComponentList.push_back(info);

	return info.componentIndex;
}

void RenderList::InitializeComponents(Microsoft::WRL::ComPtr<ID3D12Device5> device, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList)
{
	for (unsigned int i = 0; i < this->renderComponentList.size(); i++)
	{
		RenderComponent* renderComponent = renderComponentList[i].renderComponent;

		renderComponent->InitializeComponent(device, commandList);
	}
}

void RenderList::DrawAllComponents(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList, Camera camera)
{
	for (unsigned int i = 0; i < this->renderComponentList.size(); i++)
	{
		RenderComponent* renderComponent = this->renderComponentList[i].renderComponent;

		renderComponent->Draw(commandList, camera);
	}
}
