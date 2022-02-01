#include "RenderList.h"
#include <thread>

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

UINT64 RenderList::AssignRenderComponent2D(RenderComponent2D* renderComponent2d)
{
	this->renderComponent2DList.push_back(RenderComponent2DInfo());
	RenderComponent2DInfo* info = &this->renderComponent2DList[this->renderComponent2DList.size() - 1];

	info->enable = Boolean(true, &this->stateEnableChange2D);
	info->componentIndex = this->currentRenderComponent2DIndex++;
	info->renderComponent2d = renderComponent2d;

	renderComponent2d->SetEnable(&info->enable);

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

void RenderList::InitializeComponents2D(Microsoft::WRL::ComPtr<ID3D12Device5> device)
{
	for (unsigned int i = 0; i < this->renderComponent2DList.size(); i++)
	{
		RenderComponent2D* renderComponent2d = renderComponent2DList[i].renderComponent2d;

		renderComponent2d->Initialize(device);
	}

	this->stateEnableChange2D = false;
}

void RenderList::InitializeAllBundleLists()
{
	for (unsigned int i = 0; i < this->renderComponentList.size(); i++)
	{
		RenderComponent* renderComponent = renderComponentList[i].renderComponent;

		renderComponent->InitializeBundleListForRender();
	}

	this->stateEnableChange2D;
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

void DrawRenderComponents(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList, std::vector<RenderComponentInfo> renderComponents, unsigned int start, unsigned int size, Camera camera)
{
	for (unsigned int i = start; i < (start + size); i++)
	{
		if (renderComponents[i].enable.GetBool())
		{
			RenderComponent* renderComponent = renderComponents[i].renderComponent;

			renderComponent->Draw(commandList, camera);
		}
	}
}

void RenderList::DrawAllComponents(std::vector<ID3D12GraphicsCommandList4*> commandList, Camera camera)
{
	// Calculate the assignment of render components to draw given the amount of threads.
	unsigned int renderComponentsPresent = this->renderComponentList.size();
	unsigned int commandListPresent = commandList.size();

	unsigned int oneExtraTaskPerThread = renderComponentsPresent % commandListPresent;
	unsigned int minimumTasksAllotedPerThread = renderComponentsPresent / commandListPresent;

	// Loop to draw all the components using threads.
	static std::vector<std::thread> thread_objects;
	thread_objects.reserve(commandListPresent);

	unsigned int start = 0;
	unsigned int size = 0;

	for(unsigned int i = 0; i < commandListPresent && (minimumTasksAllotedPerThread != 0 || oneExtraTaskPerThread != 0); i++)
	{
		// Calculate the render component alignment.
		start = start + size;

		if (oneExtraTaskPerThread != 0)
		{
			oneExtraTaskPerThread--;
			size = minimumTasksAllotedPerThread + 1;
		}
		else
		{
			size = minimumTasksAllotedPerThread;
		}

		// Spawn threads.
		thread_objects.emplace_back(DrawRenderComponents, commandList[i], std::ref(this->renderComponentList), start, size, camera);
	}
	
	// Wait for each thread to rejoin.
	for (unsigned int i = 0; i < thread_objects.size(); i++)
	{
		if(thread_objects[i].joinable())
			thread_objects[i].join();
	}
}

void RenderList::DrawAll2DComponents(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList, Camera2D camera)
{
	for (unsigned int i = 0; i < this->renderComponent2DList.size(); i++)
	{
		if (renderComponent2DList[i].enable.GetBool())
		{
			RenderComponent2D* renderComponent2d = this->renderComponent2DList[i].renderComponent2d;

			renderComponent2d->Draw(commandList, camera);
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
