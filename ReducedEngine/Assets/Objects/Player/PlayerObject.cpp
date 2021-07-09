#include "PlayerObject.h"

#include "../Output/Output.h"
#include "../Input/Input.h"
#include "../Time.h"

PlayerObject::PlayerObject()
{
	this->camera.width = Output::GetInstance()->GetWindowWidth();
	this->camera.height = Output::GetInstance()->GetWindowHeight();

	MainCamera::GetInstance()->SetNewCamera(&this->camera);

	this->objectTransform.SetPosition(0.0f, 0.0f, 3.0f);
}

void PlayerObject::Update()
{
	MouseClass* mouse = Input::GetInstance()->GetMouse();
	KeyboardClass* keyboard = Input::GetInstance()->GetKeyboard();
	Time time = *Time::GetInstance();

	// Rotate the camera left and right by moving the mouse horizontally.
	// And, rotate the camera up and down by moving the mouse up and down.
	while (!mouse->EventBufferIsEmpty())
	{
		MouseEvent me = mouse->ReadEvent();
		if (me.GetType() == MouseEvent::EventType::RAW_MOVE)
		{
			this->objectTransform.RotateX(-(float)me.GetPosX() * this->camera_rotation_speed * time.GetDeltaTime());
			this->objectTransform.RotateY((float)me.GetPosY() * this->camera_rotation_speed * time.GetDeltaTime());
		}
	}

	float delta_movement_speed = this->camera_move_speed * time.GetDeltaTime();

	// Move forward.
	if (keyboard->KeyIsPressed('W'))
	{
		this->objectTransform.Translate(this->objectTransform.GetLocalForward() * this->camera_move_speed);
	}
	// Move backward.
	if (keyboard->KeyIsPressed('S'))
	{
		this->objectTransform.Translate(this->objectTransform.GetLocalBackward() * this->camera_move_speed);
	}
	// Move left.
	if (keyboard->KeyIsPressed('D'))
	{
		this->objectTransform.Translate(this->objectTransform.GetLocalLeft() * this->camera_move_speed);
	}
	// Move right.
	if (keyboard->KeyIsPressed('A'))
	{
		this->objectTransform.Translate(this->objectTransform.GetLocalRight() * this->camera_move_speed);
	}
}
