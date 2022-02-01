#include "UniversalDescriptorHeap.h"
#include "../ErrorLogger.h"

void UniversalDescriptorHeap::Initialize(Microsoft::WRL::ComPtr<ID3D12Device5
> device)
{
	this->srvCbvUavHeap.Initialize(device, this->maxSrvCbvUavHeapCount, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
	this->samplerHeap.Initialize(device, this->maxSamplerHeapCount, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
}

UINT64 UniversalDescriptorHeap::SetCpuHandle(Microsoft::WRL::ComPtr<ID3D12Device5> device, Microsoft::WRL::ComPtr<ID3D12Resource>* resource, D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc)
{
	UINT64 currentIndex = this->currentSrvCbvUavNum++;

	if (currentIndex > this->maxSrvCbvUavHeapCount)
	{
		ExitWithMessage("ERROR: Universal Descriptor Heap, heap count exceeded maximum value for shader resource view.\n");
	}

	device->CreateShaderResourceView(resource->Get(), srvDesc, this->srvCbvUavHeap.GetCPUHandle(currentIndex));

	return currentIndex;
}

UINT64 UniversalDescriptorHeap::SetCpuHandle(Microsoft::WRL::ComPtr<ID3D12Device5> device, ID3D12Resource* resource, D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc)
{
	UINT64 currentIndex = this->currentSrvCbvUavNum++;

	if (currentIndex > this->maxSrvCbvUavHeapCount)
	{
		ExitWithMessage("ERROR: Universal Descriptor Heap, heap count exceeded maximum value for shader resource view.\n");
	}

	device->CreateShaderResourceView(resource, srvDesc, this->srvCbvUavHeap.GetCPUHandle(currentIndex));

	return currentIndex;
}

UINT64 UniversalDescriptorHeap::SetCpuHandle(Microsoft::WRL::ComPtr<ID3D12Device5> device, D3D12_CONSTANT_BUFFER_VIEW_DESC* cbvDesc)
{
	UINT64 currentIndex = this->currentSrvCbvUavNum++;

	if (currentIndex > this->maxSrvCbvUavHeapCount)
	{
		ExitWithMessage("ERROR: Universal Descriptor Heap, heap count exceeded maximum value for constant buffer view.\n");
	}

	device->CreateConstantBufferView(cbvDesc, this->srvCbvUavHeap.GetCPUHandle(currentIndex));

	return currentIndex;
}

UINT64 UniversalDescriptorHeap::SetCpuHandle(Microsoft::WRL::ComPtr<ID3D12Device5> device, Microsoft::WRL::ComPtr<ID3D12Resource>* resource, Microsoft::WRL::ComPtr<ID3D12Resource>* pCounterResource, D3D12_UNORDERED_ACCESS_VIEW_DESC* uavDesc)
{
	UINT64 currentIndex = this->currentSrvCbvUavNum++;

	if (currentIndex > this->maxSrvCbvUavHeapCount)
	{
		ExitWithMessage("ERROR: Universal Descriptor Heap, heap count exceeded maximum value unordered access view.\n");
	}

	device->CreateUnorderedAccessView(resource->Get(), pCounterResource->Get(), uavDesc, this->srvCbvUavHeap.GetCPUHandle(currentIndex));

	return currentIndex;
}

UINT64 UniversalDescriptorHeap::SetCpuHandle(Microsoft::WRL::ComPtr<ID3D12Device5> device, D3D12_SAMPLER_DESC* samplerDesc)
{
	UINT64 currentIndex = this->currentSamplerNum++;

	if (currentIndex > this->maxSamplerHeapCount)
	{
		ExitWithMessage("ERROR: Universal Descriptor Heap, heap count exceeded maximum value sampler view.\n");
	}

	device->CreateSampler(samplerDesc, this->samplerHeap.GetCPUHandle(currentIndex));

	return currentIndex;
}

D3D12_GPU_DESCRIPTOR_HANDLE UniversalDescriptorHeap::GetCbvSrvUavGPUHandle(UINT64 index)
{
	if (index > this->maxSrvCbvUavHeapCount)
	{
		ExitWithMessage("ERROR: Universal Descriptor Heap, requested index exceeded the maximum value for the shader resource, constant or unordered access view heap.\n");
	}

	return this->srvCbvUavHeap.GetGPUHandle(index);
}

D3D12_GPU_DESCRIPTOR_HANDLE UniversalDescriptorHeap::GetSamplerGPUHandle(UINT64 index)
{
	if (index > this->maxSamplerHeapCount)
	{
		ExitWithMessage("ERROR: Universal Descriptor Heap, requested index exceeded the maximum value for the sampler view heap.\n");
	}

	return this->samplerHeap.GetGPUHandle(index);
}

void UniversalDescriptorHeap::AssignToGraphicsCommandList(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList)
{
	ID3D12DescriptorHeap* ppHeaps[] = { this->srvCbvUavHeap.Get(), this->samplerHeap.Get() };
	commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
}

void UniversalDescriptorHeap::Release()
{
	this->srvCbvUavHeap.Release();
	this->samplerHeap.Release();
}
