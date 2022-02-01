#include "CameraObject2D.h"

void CameraObject2D::InitializeCameraObject(Transform2D transform)
{
	this->objectTransform.SetScale(20.0f, 20.0f);

	this->camera.width = this->objectTransform.GetScale().X() * Output::GetInstance()->GetWindowWidth() / Output::GetInstance()->GetWindowHeight();
	this->camera.height = this->objectTransform.GetScale().Y() * 1.0f;

	MainCamera2D::GetInstance()->SetNewCamera2D(&this->camera);
}

void CameraObject2D::HandleKeyInput()
{
	// Move up.
	if (keyboard->KeyIsPressed('W'))
	{
		this->objectTransform.Translate(this->objectTransform.GetLocalUp() * this->camera_move_speed);
	}
	// Move down.
	if (keyboard->KeyIsPressed('S'))
	{
		this->objectTransform.Translate(this->objectTransform.GetLocalDown() * this->camera_move_speed);
	}
	// Move left.
	if (keyboard->KeyIsPressed('A'))
	{
		this->objectTransform.Translate(this->objectTransform.GetLocalLeft() * this->camera_move_speed);
	}
	// Move right.
	if (keyboard->KeyIsPressed('D'))
	{
		this->objectTransform.Translate(this->objectTransform.GetLocalRight() * this->camera_move_speed);
	}
}

void CameraObject2D::HandleControllerInput()
{
	XInput_ButtonID button_id = {};
	if (this->controller->GetButtonPressed(button_id.Dpad_Left))
	{
		this->objectTransform.Translate(this->objectTransform.GetLocalLeft() * this->camera_move_speed);
	}
	if (this->controller->GetButtonPressed(button_id.Dpad_Right))
	{
		this->objectTransform.Translate(this->objectTransform.GetLocalRight() * this->camera_move_speed);
	}
	if (this->controller->GetButtonPressed(button_id.Dpad_Up))
	{
		this->objectTransform.Translate(this->objectTransform.GetLocalUp() * this->camera_move_speed);
	}
	if (this->controller->GetButtonPressed(button_id.Dpad_Down))
	{
		this->objectTransform.Translate(this->objectTransform.GetLocalDown() * this->camera_move_speed);
	}
	if (this->controller->GetButtonDown(button_id.B))
	{
		Output::GetInstance()->CloseWindow();
	}
}

CameraObject2D::CameraObject2D()
{
	this->InitializeCameraObject(Transform2D());
}

CameraObject2D::CameraObject2D(Transform2D transform)
{
	this->InitializeCameraObject(Transform2D());
}

void CameraObject2D::FollowPosition(Transform2D* transform)
{
	this->followTransformPosition = transform;
}

void CameraObject2D::Update()
{
	if (this->followTransformPosition)
	{
		this->objectTransform.SetPosition(this->followTransformPosition->GetGlobalPostion());
	}
}
