#pragma once

#include "../MathEngine.h"

// Function that contains the class definition for camera component along with the single instance MainCamera to be used for rendering.

// Camera component class.
class Camera
{
private:
	Transform* transform = nullptr;

public:
	float fieldOfView = 45.0f;
	float depthNear = 0.1f;
	float depthFar = 1000.0f;
	unsigned int width = 500;
	unsigned int height = 500;

	Matrix4 view;
	Matrix4 projection;

public:
	Camera(Transform* transform) : transform(transform) {}

	// Function to calculate the view and projection matrix.
	void CalculateViewProjection();

	// Function to get view projection matrix.
	Matrix4 GetViewProjectionMatrix();
};

// Singleton(single instance) definition of Main Camera component.
class MainCamera
{
private:
	MainCamera() {}

	// Assigned main camera.
	Camera* mainCamera = nullptr;

public:
	// Function to get the single instance of the MainCamera.
	static MainCamera* GetInstance()
	{
		static MainCamera* instance = new MainCamera();
		return instance;
	}

	// Function to set a new main camera.
	void SetNewCamera(Camera* camera);
	// Function to get the main camera.
	Camera GetCamera();

	// Get functions for respective camera characteristics.
	Matrix4 GetCameraMatrix();
	Matrix4 GetProjectionMatrix();
	Matrix4 GetViewMatrix();
	unsigned int GetWidth();
	unsigned int GetHeight();
	float GetNear();
	float GetFar();
	float GetFieldOfView();
	void CalculateViewProjection();
};