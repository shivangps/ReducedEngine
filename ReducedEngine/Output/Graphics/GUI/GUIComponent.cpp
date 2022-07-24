#include "GUIComponent.h"
#include "../ErrorLogger.h"

UINT64 GUIDescriptorHeap::GetNewIndexForSrvCbvUav()
{
	UINT64 resultCurrentIndex = 0;

	if (this->releasedCbvSrvUavIndices.size() != 0)
	{
		resultCurrentIndex = this->releasedCbvSrvUavIndices[this->releasedCbvSrvUavIndices.size() - 1];
		this->releasedCbvSrvUavIndices.pop_back();
	}
	else
	{
		resultCurrentIndex = this->currentSrvCbvUavNum++;
	}

	return resultCurrentIndex;
}

UINT64 GUIDescriptorHeap::GetNewIndexForSampler()
{
	UINT64 resultCurrentIndex = 0;

	if (this->releasedSamplerIndices.size() != 0)
	{
		resultCurrentIndex = this->releasedSamplerIndices[this->releasedSamplerIndices.size() - 1];
		this->releasedSamplerIndices.pop_back();
	}
	else
	{
		resultCurrentIndex = this->currentSamplerNum++;
	}

	return resultCurrentIndex;
}

void GUIDescriptorHeap::Initialize(Microsoft::WRL::ComPtr<ID3D12Device5> device)
{
	this->srvCbvUavHeap.Initialize(device, this->maxSrvCbvUavHeapCount, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
	this->samplerHeap.Initialize(device, this->maxSamplerHeapCount, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
}

UINT64 GUIDescriptorHeap::SetCpuHandle(Microsoft::WRL::ComPtr<ID3D12Device5> device, Microsoft::WRL::ComPtr<ID3D12Resource>* resource, D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc)
{
	UINT64 currentIndex = this->GetNewIndexForSrvCbvUav();

	if (currentIndex > this->maxSrvCbvUavHeapCount)
	{
		ExitWithMessage("ERROR: GUI Descriptor Heap, heap count exceeded maximum value for shader resource view.\n");
	}

	device->CreateShaderResourceView(resource->Get(), srvDesc, this->srvCbvUavHeap.GetCPUHandle(currentIndex));

	return currentIndex;
}

UINT64 GUIDescriptorHeap::SetCpuHandle(Microsoft::WRL::ComPtr<ID3D12Device5> device, ID3D12Resource* resource, D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc)
{
	UINT64 currentIndex = this->GetNewIndexForSrvCbvUav();

	if (currentIndex > this->maxSrvCbvUavHeapCount)
	{
		ExitWithMessage("ERROR: GUI Descriptor Heap, heap count exceeded maximum value for shader resource view.\n");
	}

	device->CreateShaderResourceView(resource, srvDesc, this->srvCbvUavHeap.GetCPUHandle(currentIndex));

	return currentIndex;
}

UINT64 GUIDescriptorHeap::SetCpuHandle(Microsoft::WRL::ComPtr<ID3D12Device5> device, D3D12_CONSTANT_BUFFER_VIEW_DESC* cbvDesc)
{
	UINT64 currentIndex = this->GetNewIndexForSrvCbvUav();

	if (currentIndex > this->maxSrvCbvUavHeapCount)
	{
		ExitWithMessage("ERROR: GUI Descriptor Heap, heap count exceeded maximum value for constant buffer view.\n");
	}

	device->CreateConstantBufferView(cbvDesc, this->srvCbvUavHeap.GetCPUHandle(currentIndex));

	return currentIndex;
}

UINT64 GUIDescriptorHeap::SetCpuHandle(Microsoft::WRL::ComPtr<ID3D12Device5> device, Microsoft::WRL::ComPtr<ID3D12Resource>* resource, Microsoft::WRL::ComPtr<ID3D12Resource>* pCounterResource, D3D12_UNORDERED_ACCESS_VIEW_DESC* uavDesc)
{
	UINT64 currentIndex = this->GetNewIndexForSrvCbvUav();

	if (currentIndex > this->maxSrvCbvUavHeapCount)
	{
		ExitWithMessage("ERROR: GUI Descriptor Heap, heap count exceeded maximum value unordered access view.\n");
	}

	device->CreateUnorderedAccessView(resource->Get(), pCounterResource->Get(), uavDesc, this->srvCbvUavHeap.GetCPUHandle(currentIndex));

	return currentIndex;
}

UINT64 GUIDescriptorHeap::SetCpuHandle(Microsoft::WRL::ComPtr<ID3D12Device5> device, D3D12_SAMPLER_DESC* samplerDesc)
{
	UINT64 currentIndex = this->GetNewIndexForSampler();

	if (currentIndex > this->maxSamplerHeapCount)
	{
		ExitWithMessage("ERROR: Universal Descriptor Heap, heap count exceeded maximum value sampler view.\n");
	}

	device->CreateSampler(samplerDesc, this->samplerHeap.GetCPUHandle(currentIndex));

	return currentIndex;
}

D3D12_CPU_DESCRIPTOR_HANDLE GUIDescriptorHeap::GetCbvSrvUavCPUHandle(UINT64 index)
{
	if (index > this->maxSrvCbvUavHeapCount)
	{
		ExitWithMessage("ERROR: Universal Descriptor Heap, requested index exceeded the maximum value for the shader resource, constant or unordered access view heap.\n");
	}

	return this->srvCbvUavHeap.GetCPUHandle(index);
}

D3D12_GPU_DESCRIPTOR_HANDLE GUIDescriptorHeap::GetCbvSrvUavGPUHandle(UINT64 index)
{
	if (index > this->maxSrvCbvUavHeapCount)
	{
		ExitWithMessage("ERROR: Universal Descriptor Heap, requested index exceeded the maximum value for the shader resource, constant or unordered access view heap.\n");
	}

	return this->srvCbvUavHeap.GetGPUHandle(index);
}

D3D12_GPU_DESCRIPTOR_HANDLE GUIDescriptorHeap::GetSamplerGPUHandle(UINT64 index)
{
	if (index > this->maxSamplerHeapCount)
	{
		ExitWithMessage("ERROR: Universal Descriptor Heap, requested index exceeded the maximum value for the sampler view heap.\n");
	}

	return this->samplerHeap.GetGPUHandle(index);
}

void GUIDescriptorHeap::AssignToGraphicsCommandList(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList)
{
	ID3D12DescriptorHeap* ppHeaps[] = { this->srvCbvUavHeap.Get(), this->samplerHeap.Get() };
	commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
}

void GUIDescriptorHeap::ReleaseCbvSrvUavDescriptor(UINT64 index)
{
	if (index > (this->currentSrvCbvUavNum - 1))
		return;

	this->releasedCbvSrvUavIndices.push_back(index);
}

void GUIDescriptorHeap::ReleaseSamplerDescriptor(UINT64 index)
{
	if (index > (this->currentSamplerNum - 1))
		return;

	this->releasedSamplerIndices.push_back(index);
}

void GUIDescriptorHeap::Release()
{
	this->srvCbvUavHeap.Release();
	this->samplerHeap.Release();
}

void GUIComponent::SetEnable(Boolean* pBool)
{
	this->enable = pBool;
}

void GUIComponent::EnableRendering()
{
	this->enable->SetTrue();
}

void GUIComponent::DisableRendering()
{
	this->enable->SetFalse();
}