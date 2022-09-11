#include "RenderComponent2D.h"
#include "../Output/Graphics/RenderList.h"
#include "../UniversalDescriptorHeap.h"

void RenderComponent2D::InitializeMatrixConstantBuffer(Microsoft::WRL::ComPtr<ID3D12Device5> device, LPCWSTR matrixConstantBufferName)
{
	this->matrixConstantBuffer.Initialize(device, sizeof(MatrixData), matrixConstantBufferName);
	UniversalDescriptorHeap* UDH = UniversalDescriptorHeap::GetInstance();
	this->matrixConstantBufferHandle = UDH->GetCbvSrvUavGPUHandle(UDH->SetCpuHandle(device, this->matrixConstantBuffer.GetConstantBufferViewDesc()));
}

void RenderComponent2D::UpdateMatrixConstantBuffer(Camera2D camera)
{
	Transform2D transform = *this->transform;

	Matrix4 model = Matrix4();

	model = model.Scale(transform.GetGlobalScale());
	model = model.Rotation(transform.GetGlobalRotation());
	model = model.Translation(transform.GetGlobalPostion());

	matrixData.cameraMatrix = model * camera.GetViewProjectionMatrix();
	this->matrixConstantBuffer.CopyDataToConstantBufferLocation(&this->matrixData);
}

void RenderComponent2D::SetEnable(Boolean* pBool)
{
	this->enable = pBool;
}

void RenderComponent2D::EnableRendering()
{
	this->enable->SetTrue();
}

void RenderComponent2D::DisableRendering()
{
	this->enable->SetFalse();
}

void RenderComponent2D::SetRenderList(RenderList* renderlist)
{
	this->associatedRenderList = renderlist;
}

bool RenderComponent2D::SetBackgroundColor(Vector3 bgColor)
{
	if (this->associatedRenderList)
	{
		this->associatedRenderList->SetBackgroundColor(bgColor);
		return true;
	}
	else
	{
		DebugLog("Render List not assigned to the Render Component 2D of Name: " + std::string(typeid(*this).name()) + " to change the background color. \n");
		return false;
	}
}

void RenderComponent2D::SetGameObject(GameObject2D* gameObject)
{
	this->associatedGameObject = gameObject;
}

GameObject2D* RenderComponent2D::GetGameObject()
{
	return this->associatedGameObject;
}
