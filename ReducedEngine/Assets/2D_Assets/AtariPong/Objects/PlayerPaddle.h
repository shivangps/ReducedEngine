#pragma once
#include "PongPaddle.h"

// Class for player controlled paddle.
class PlayerPaddle : public GameObject2D
{
private:
	SpriteRenderComponent2D* spriteRenderer = nullptr;
	BoxCollider2D boxCollider = BoxCollider2D(&this->objectTransform);
	Rigidbody2D* rigidbody = nullptr;
	
	KeyboardClass* keyboard = Input::GetInstance()->GetKeyboard();
	MouseClass* mouse = Input::GetInstance()->GetMouse();
	XController* controller = Input::GetInstance()->GetController();
	Output* output = Output::GetInstance();
	Time* time = Time::GetInstance();

	PaddleCharacteristics characteristics;

	// Object color to red to represent player.
	Vector3 color = Vector3((float) 157/255, (float) 255/255, (float) 0/255);
	// Function to move the paddle using the controller.
	void MovePaddleUsingController();
	// Function to move the paddle using the mouse.
	void MovePaddleUsingMouse();
	// Function to convert the coordinates from screen to 3D coordinates.
	Vector2 ScreenTo3DCoordinates(Vector2 ScreenCoordinates);

public:
	PlayerPaddle(Transform2D transform)
	{
		this->objectTransform = transform;
	}

	// Function to initialize the game object.
	void Initialize(RenderList* sceneRenderComponentList, PhysicsComponentList* physicsComponentList, AudioComponentList* audioComponentList, GUIComponentList* guiComponentList);
	// Function that is called every frame.
	void Update();
};