#pragma once
#include "../GameObject.h"

class PlayerObject : public GameObject
{
private:
	Camera camera = { &this->objectTransform };

	KeyboardClass* keyboard = Input::GetInstance()->GetKeyboard();
	MouseClass* mouse = Input::GetInstance()->GetMouse();
	XController* controller = Input::GetInstance()->GetController();

	Time* time = Time::GetInstance();

	// Function to handle the keyboard and mouse inputs.
	void HandleKeyboardInputs();
	// Function to handle the X-Box controller inputs.
	void HandleXBoxControllerInputs();

public:
	// Speed for the camera to rotate using mouse.
	float camera_key_rotation_speed = 0.03f;
	float camera_controller_rotation_speed = 0.3f;
	// Speed for the camera to move using keys.
	float camera_key_move_speed = 0.1f;
	float camera_controller_move_speed = 0.01f;

	PlayerObject();

	void Update();
};