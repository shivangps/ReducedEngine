#include "RenderComponent2D.h"
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

	Matrix4 model;

	model = model.Rotation(transform.GetGlobalRotation());
	model = model.Scale(transform.GetGlobalScale());
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