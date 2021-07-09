#include "Camera.h"

void Camera::CalculateViewProjection()
{
	Transform cachedTransform = *this->transform;
	// VIEW
	view = DirectX::XMMatrixLookAtLH(
		cachedTransform.GetGlobalPostion().GetVector(), 
		(cachedTransform.GetGlobalPostion() + cachedTransform.GetLocalForward()).GetVector(), 
		cachedTransform.GetLocalUp().GetVector());

	// PROJECTION
	projection = DirectX::XMMatrixPerspectiveFovLH(this->fieldOfView, (float)this->width / (float)this->height, this->depthNear, this->depthFar);
}

Matrix4 Camera::GetViewProjectionMatrix()
{
	return (view * projection);
}

Transform Camera::GetTransform()
{
	return *this->transform;
}

void MainCamera::SetNewCamera(Camera* camera)
{
	this->mainCamera = camera;
}

Camera MainCamera::GetCamera()
{
	return *this->mainCamera;
}

Transform MainCamera::GetTransform()
{
	return this->cameraTransform;
}

Matrix4 MainCamera::GetCameraMatrix()
{
	return this->cameraMatrix;
}

Matrix4 MainCamera::GetProjectionMatrix()
{
	return this->projectionMatrix;
}

Matrix4 MainCamera::GetViewMatrix()
{
	return this->viewMatrix;
}

unsigned int MainCamera::GetWidth()
{
	return this->mainCamera->width;
}

unsigned int MainCamera::GetHeight()
{
	return this->mainCamera->height;
}

float MainCamera::GetNear()
{
	return this->mainCamera->depthNear;
}

float MainCamera::GetFar()
{
	return this->mainCamera->depthFar;
}

float MainCamera::GetFieldOfView()
{
	return this->mainCamera->fieldOfView;
}

void MainCamera::CalculateViewProjection()
{
	this->mainCamera->CalculateViewProjection();
	this->cameraMatrix = this->mainCamera->GetViewProjectionMatrix();
	this->viewMatrix = this->mainCamera->view;
	this->projectionMatrix = this->mainCamera->projection;
	this->cameraTransform = this->GetTransform();
}
