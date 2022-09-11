#pragma once
#include "../GameObject2D.h"

// Game object in 2D that handles the play button in menu scene.

class SpriteRenderComponent2D;

class PlayButtonObject : public GameObject2D
{
private:
	XController* controller = Input::GetInstance()->GetController();
	MouseClass* mouse = Input::GetInstance()->GetMouse();
	Output* output = Output::GetInstance();

	SpriteRenderComponent2D* renderer = nullptr;
	SpriteRenderComponent2D* textRenderer = nullptr;

	Vector3 buttonColor = Vector3(0.0f, 0.0f, 0.7f);
	Vector3 textColor = Vector3(1.0f, 1.0f, 1.0f);

	bool isClicked = false;

	Transform2D defaultState = Transform2D();
	Transform2D hoverState = Transform2D();
	Transform2D heldState = Transform2D();

	Vector2 ScreenToCoordinates(Vector2 ScreenCoordinates);

public:
	PlayButtonObject(Transform2D transform)
	{
		this->objectTransform = transform;
	}

	// Function to initialize the game object.
	void Initialize(RenderList* sceneRenderComponentList, PhysicsComponentList* physicsComponentList, AudioComponentList* audioComponentList, GUIComponentList* guiComponentList);
	// Function call for updating the game object state.
	void Update();
	// Function to get if the button is clicked.
	bool GetClickState();
};