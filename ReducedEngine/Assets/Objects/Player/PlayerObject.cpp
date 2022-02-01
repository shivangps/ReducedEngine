#include "PlayerObject.h"

#include "../Output/Output.h"
#include "../Input/Input.h"
#include "../Time.h"

void PlayerObject::HandleKeyboardInputs()
{
	// Rotate the camera left and right by moving the mouse horizontally.
// And, rotate the camera up and down by moving the mouse up and down.
	while (!mouse->EventBufferIsEmpty())
	{
		MouseEvent me = mouse->ReadEvent();
		if (me.GetType() == MouseEvent::EventType::RAW_MOVE)
		{
			this->objectTransform.RotateX(-(float)me.GetPosX() * this->camera_key_rotation_speed * time->GetDeltaTime());
			this->objectTransform.RotateY((float)me.GetPosY() * this->camera_key_rotation_speed * time->GetDeltaTime());
		}
	}

	float delta_movement_speed = this->camera_key_move_speed * time->GetDeltaTime();

	// Move forward.
	if (keyboard->KeyIsPressed('W'))
	{
		this->objectTransform.Translate(this->objectTransform.GetLocalForward() * this->camera_key_move_speed);
	}
	// Move backward.
	if (keyboard->KeyIsPressed('S'))
	{
		this->objectTransform.Translate(this->objectTransform.GetLocalBackward() * this->camera_key_move_speed);
	}
	// Move left.
	if (keyboard->KeyIsPressed('D'))
	{
		this->objectTransform.Translate(this->objectTransform.GetLocalLeft() * this->camera_key_move_speed);
	}
	// Move right.
	if (keyboard->KeyIsPressed('A'))
	{
		this->objectTransform.Translate(this->objectTransform.GetLocalRight() * this->camera_key_move_speed);
	}
}

void PlayerObject::HandleXBoxControllerInputs()
{
	XInput_ButtonID button_id = {};

	float deltaTime = this->time->GetDeltaTime();

	// Movement.
	this->objectTransform.Translate(this->objectTransform.GetLocalForward() * this->controller->GetLeftThumbNormalizedY() * this->controller->GetLeftThumbNormalizedMagnitude() * this->camera_controller_move_speed * deltaTime);
	this->objectTransform.Translate(this->objectTransform.GetLocalLeft() * this->controller->GetLeftThumbNormalizedX() * this->controller->GetLeftThumbNormalizedMagnitude() * this->camera_controller_move_speed * deltaTime);

	// Rotation.
	this->objectTransform.RotateX(this->controller->GetRightThumbNormalizedY() * this->controller->GetRightThumbNormalizedMagnitude() * this->camera_controller_rotation_speed * deltaTime);
	this->objectTransform.RotateY(this->controller->GetRightThumbNormalizedX() * this->controller->GetRightThumbNormalizedMagnitude() * this->camera_controller_rotation_speed * deltaTime);

	// Quiting. (Press B to quit window).
	if (this->controller->GetButtonDown(button_id.B))
	{
		Output::GetInstance()->CloseWindow();
	}
}

PlayerObject::PlayerObject()
{
	this->camera.width = Output::GetInstance()->GetWindowWidth();
	this->camera.height = Output::GetInstance()->GetWindowHeight();

	MainCamera::GetInstance()->SetNewCamera(&this->camera);

	this->objectTransform.SetPosition(0.0f, 0.0f, 5.0f);
}

void PlayerObject::Update()
{
	this->HandleKeyboardInputs();
	this->HandleXBoxControllerInputs();
}
