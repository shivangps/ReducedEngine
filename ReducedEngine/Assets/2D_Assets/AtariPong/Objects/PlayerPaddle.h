#pragma once
#include "PongPaddle.h"

// Class for player controlled paddle.
class PlayerPaddle : public GameObject2D
{
private:	
	WireframeRenderComponent2D* renderer = nullptr;
	BoxCollider2D boxCollider = BoxCollider2D(&this->objectTransform);
	Rigidbody2D* rigidbody = nullptr;
	
	KeyboardClass* keyboard = Input::GetInstance()->GetKeyboard();
	XController* controller = Input::GetInstance()->GetController();
	Output* output = Output::GetInstance();
	Time* time = Time::GetInstance();

	PaddleCharacteristics characteristics;

	// Object color to red to represent player.
	Vector3 color = Vector3(1.0f, 1.0f, 1.0f);
	// Function to move the paddle using the controller.
	void MovePaddleUsingController();

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