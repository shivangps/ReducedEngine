#pragma once
#include "../../../../GameObject2D.h"

// Game object that controls camera.

class CameraObject2D : public GameObject2D
{
private:
	Camera2D camera = {&this->objectTransform};

	MouseClass* mouse = Input::GetInstance()->GetMouse();
	KeyboardClass* keyboard = Input::GetInstance()->GetKeyboard();
	XController* controller = Input::GetInstance()->GetController();
	Time* time = Time::GetInstance();

	Transform2D* followTransformPosition;

	void InitializeCameraObject(Transform2D transform);

	// Function to handle key inputs to move the camera.
	void HandleKeyInput();
	// Function to handle the controller input to move the camera.
	void HandleControllerInput();

public:
	// Speed for the camera to move using keys and controller.
	float camera_move_speed = 0.1f;

	CameraObject2D();
	CameraObject2D(Transform2D transform);

	// Function for the camera to follow the position of the transform.
	void FollowPosition(Transform2D* transform);

	void Update();
};