#pragma once
#include "../Helper.h"
#include "../GraphicsHelper.h"
#include "../MathEngine.h"

// Class responsible for the management of descriptor heap for GUI elements.
class GUIDescriptorHeap
{
private:
	GUIDescriptorHeap() {}

	// Heap that will store alll the view of shader resource as well as constant buffer view.
	DescriptorHeap srvCbvUavHeap = {};
	DescriptorHeap samplerHeap = {};

	// Maximum number of descriptors the heap can store.
	const UINT64 maxSrvCbvUavHeapCount = 1000000;
	const UINT64 maxSamplerHeapCount = 2048;

	// Currently number of descriptors stored in heap.
	UINT64 currentSrvCbvUavNum = 0;
	UINT64 currentSamplerNum = 0;

	// Dynamic array to store the invalid gpu descriptor handle index.
	std::vector<UINT64> releasedCbvSrvUavIndices = {};
	std::vector<UINT64> releasedSamplerIndices = {};

	// Function to get a new index for the cbv, srv, uav descriptor heap.
	UINT64 GetNewIndexForSrvCbvUav();
	// Function to get a new index for the sampler descriptor heap.
	UINT64 GetNewIndexForSampler();

public:
	// Retruns a single instance of this class.
	static GUIDescriptorHeap* GetInstance()
	{
		static GUIDescriptorHeap* instance = new GUIDescriptorHeap();
		return instance;
	}

	// Initialize the GUI descriptor heap.
	void Initialize(Microsoft::WRL::ComPtr<ID3D12Device5> device);

	// Function to set a new srv or cbv and get the index of the srv or sbc stored in the heap.
	UINT64 SetCpuHandle(Microsoft::WRL::ComPtr<ID3D12Device5> device, Microsoft::WRL::ComPtr<ID3D12Resource>* resource, D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc);
	UINT64 SetCpuHandle(Microsoft::WRL::ComPtr<ID3D12Device5> device, ID3D12Resource* resource, D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc);
	UINT64 SetCpuHandle(Microsoft::WRL::ComPtr<ID3D12Device5> device, D3D12_CONSTANT_BUFFER_VIEW_DESC* cbvDesc);
	UINT64 SetCpuHandle(Microsoft::WRL::ComPtr<ID3D12Device5> device, Microsoft::WRL::ComPtr<ID3D12Resource>* resource, Microsoft::WRL::ComPtr<ID3D12Resource>* pCounterResource, D3D12_UNORDERED_ACCESS_VIEW_DESC* uavDesc);
	UINT64 SetCpuHandle(Microsoft::WRL::ComPtr<ID3D12Device5> device, D3D12_SAMPLER_DESC* samplerDesc);

	// Function to get the srv or cbv cpu handle.
	D3D12_CPU_DESCRIPTOR_HANDLE GetCbvSrvUavCPUHandle(UINT64 index);

	// Function to get the srv or cbv gpu handle.
	D3D12_GPU_DESCRIPTOR_HANDLE GetCbvSrvUavGPUHandle(UINT64 index);
	D3D12_GPU_DESCRIPTOR_HANDLE GetSamplerGPUHandle(UINT64 index);

	// Function to assign the heap to the command list.
	void AssignToGraphicsCommandList(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList);

	// Function to release a particular descriptor slot for srv, cbv and uav.
	void ReleaseCbvSrvUavDescriptor(UINT64 index);
	// Function to release a particular descriptor slot for sampler.
	void ReleaseSamplerDescriptor(UINT64 index);
	// Function to release all the descriptor heaps.
	void Release();
};

// Class that is a base virtual class for rendering the UI elements and components on screen.
class GUIComponent
{
protected:
	// Boolean to keep track if the gui component is to be rendered or not.
	Boolean* enable = nullptr;

	Transform2D* transform = nullptr;

	struct GUITransformationData
	{
		Matrix4 TransformationMatrix;
	};
	GUITransformationData transformationData;
	ConstantBuffer transformationConstantBuffer;

public:
	GUIComponent(Transform2D* transform)
	{
		this->transform = transform;
	}

	// Function to initialize the GUI com ponent.
	virtual void InitializeGUI(Microsoft::WRL::ComPtr<ID3D12Device5> device, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList) {}

	// Function to draw the GUI on the framebuffer.
	virtual void DrawGUI(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList, Matrix4 projection, unsigned int width, unsigned int height) {}

	// Boolean settings.
	void SetEnable(Boolean* pBool);
	void EnableRendering();
	void DisableRendering();
};