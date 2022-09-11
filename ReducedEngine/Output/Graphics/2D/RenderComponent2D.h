#pragma once
#include "../GraphicsHelper.h"
#include "../../../MathEngine.h"
#include "../Shader.h"
#include "../../../Helper.h"
#include "Camera2D.h"

// Class describes the generic base class for render components in 2D space.

class RenderComponent2D
{
protected:
	// Boolean to keep track if the render component is to be rendered or not.
	Boolean* enable = nullptr;

	Transform2D* transform = nullptr;

	ConstantBuffer matrixConstantBuffer = {};
	D3D12_GPU_DESCRIPTOR_HANDLE matrixConstantBufferHandle = {};

	class RenderList* associatedRenderList = nullptr;

	struct MatrixData
	{
		Matrix4 cameraMatrix;
	}matrixData;

	class GameObject2D* associatedGameObject = nullptr;

	// Function to handle the constant buffer for matrix calculation.
	void InitializeMatrixConstantBuffer(Microsoft::WRL::ComPtr<ID3D12Device5> device, LPCWSTR constantBufferName);
	// Function to update matrix constant buffer.
	void UpdateMatrixConstantBuffer(Camera2D camera);

public:
	RenderComponent2D(Transform2D* transform)
	{
		this->transform = transform;
	}

	// Virtual funstion to initialize the 2D render component.
	virtual void Initialize(Microsoft::WRL::ComPtr<ID3D12Device5> device) {}
	// Virtual function to draw the 2D sprite or geometry on render target.
	virtual void Draw(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> commandList, Camera2D camera) {}

	// Boolean settings.
	void SetEnable(Boolean* pBool);
	void EnableRendering();
	void DisableRendering();
	void SetRenderList(class RenderList* renderlist);
	bool SetBackgroundColor(Vector3 bgColor);

	void SetGameObject(GameObject2D* gameObject);
	GameObject2D* GetGameObject();
};