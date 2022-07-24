#include "GUIComponentList.h"

UINT64 GUIComponentList::AssignGUIComponent(GUIComponent* guiComponent)
{
	this->guiComponentList.push_back(GUIComponentInfo());
	GUIComponentInfo* info = &this->guiComponentList[this->guiComponentList.size() - 1];

	info->enable = Boolean(true, &this->stateEnableChange);
	info->guiComponentIndex = this->currentGUIComponentIndex++;
	info->guiComponent = guiComponent;

	guiComponent->SetEnable(&info->enable);

	return info->guiComponentIndex;
}

void GUIComponentList::InitializeAllGUIComponents(Microsoft::WRL::ComPtr<ID3D12Device5> device, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList)
{
	for (unsigned int i = 0; i < this->guiComponentList.size(); i++)
	{
		GUIComponent* guiComponent = this->guiComponentList[i].guiComponent;

		guiComponent->InitializeGUI(device, commandList);
	}

	this->stateEnableChange = false;
}

void GUIComponentList::UpdateAllGUIComponents(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList, Matrix4 projection, unsigned int width, unsigned int height)
{
	for (unsigned int i = 0; i < this->guiComponentList.size(); i++)
	{
		if (this->guiComponentList[i].enable.GetBool())
		{
			GUIComponent* guiComponent = this->guiComponentList[i].guiComponent;

			guiComponent->DrawGUI(commandList, projection, width, height);
		}
	}
}
