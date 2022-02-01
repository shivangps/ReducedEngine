#pragma once
#include "../MathEngine.h"

// Function that contains the class definition for camera component along with the single instance MainCamera2D to be used for rendering.

class Camera2D
{
private:
	Transform2D* transform = nullptr;

public:
	float depthNear = 0.1f;
	float depthFar = 50.0f;
	float zDistance = -10.0f;
	float width = 500.0f;
	float height = 500.0f;

	Matrix4 projection;
	Matrix4 view;

public:
	Camera2D(Transform2D* transform) : transform(transform) {}

	// Function to calculate the view and projection matrix.
	void CalculateViewProjection();

	// Function to get view projection matrix.
	Matrix4 GetViewProjectionMatrix();

	// Function to get the transform.
	Transform2D GetTransform();
};

// Singleton(single instance) definition of Main Camera 2D component.
class MainCamera2D
{
private:
	MainCamera2D() {}

	// Copy data of the camera.
	Matrix4 cameraMatrix;
	Matrix4 viewMatrix;
	Matrix4 projectionMatrix;
	Transform2D cameraTransform;

	// Assigned main camera.
	Camera2D* mainCamera = nullptr;

public:
	// Function to get the single instance of the MainCamera.
	static MainCamera2D* GetInstance()
	{
		static MainCamera2D* instance = new MainCamera2D();
		return instance;
	}

	// Function to set a new main camera.
	void SetNewCamera2D(Camera2D* camera);
	// Function to get the main camera.
	Camera2D GetCamera2D();

	// Get functions for respective camera characteristics.
	Transform2D GetTransform2D();
	Matrix4 GetCameraMatrix();
	Matrix4 GetProjectionMatrix();
	Matrix4 GetViewMatrix();
	unsigned int GetWidth();
	unsigned int GetHeight();
	float GetNear();
	float GetFar();
	void CalculateViewProjection();
};