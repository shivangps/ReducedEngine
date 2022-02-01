#include "Camera2D.h"

void Camera2D::CalculateViewProjection()
{
	Transform2D cachedTransform = *this->transform;
	// VIEW
	view = DirectX::XMMatrixLookAtLH(
		(Vector3(cachedTransform.GetGlobalPostion()) + Vector3(0.0f, 0.0f, this->zDistance)).GetVector(),
		(Vector3(cachedTransform.GetGlobalPostion()) + global_forward).GetVector(),
		global_up);

	// PROJECTION
	projection = DirectX::XMMatrixOrthographicLH(this->width, this->height, this->depthNear, this->depthFar);
}

Matrix4 Camera2D::GetViewProjectionMatrix()
{
	return (view * projection);
}

Transform2D Camera2D::GetTransform()
{
	return *this->transform;
}

void MainCamera2D::SetNewCamera2D(Camera2D* camera)
{
	this->mainCamera = camera;
}

Camera2D MainCamera2D::GetCamera2D()
{
	return *this->mainCamera;
}

Transform2D MainCamera2D::GetTransform2D()
{
	return this->cameraTransform;
}

Matrix4 MainCamera2D::GetCameraMatrix()
{
	return this->cameraMatrix;
}

Matrix4 MainCamera2D::GetProjectionMatrix()
{
	return this->projectionMatrix;
}

Matrix4 MainCamera2D::GetViewMatrix()
{
	return this->viewMatrix;
}

unsigned int MainCamera2D::GetWidth()
{
	return this->mainCamera->width;
}

unsigned int MainCamera2D::GetHeight()
{
	return this->mainCamera->height;
}

float MainCamera2D::GetNear()
{
	return this->mainCamera->depthNear;
}

float MainCamera2D::GetFar()
{
	return this->mainCamera->depthFar;
}

void MainCamera2D::CalculateViewProjection()
{
	this->mainCamera->CalculateViewProjection();
	this->cameraMatrix = this->mainCamera->GetViewProjectionMatrix();
	this->viewMatrix = this->mainCamera->view;
	this->projectionMatrix = this->mainCamera->projection;
	this->cameraTransform = this->GetTransform2D();
}
