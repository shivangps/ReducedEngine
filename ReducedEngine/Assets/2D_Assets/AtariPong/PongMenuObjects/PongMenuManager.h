#pragma once
#include "../GameObject2D.h"
#include "../Output/Graphics/GUI/TextGUIComponent.h"
#include "../Output/Graphics/GUI/ButtonGUIComponent.h"

class PongMenuManager : public GameObject2D
{
private:
	class PongScene* aiGameScene = nullptr;

	Vector3 backgroundColor = Vector3(0.7f, 0.9f, 0.0f);

	class PongGameManager* gameManager = nullptr;

	class PlayButtonObject* playButtonObject = nullptr;

	// PONG Text characteristics
	Transform2D PONG_transform = Transform2D();
	TextGUIComponent PONG_Text = TextGUIComponent(&this->PONG_transform);
	Vector3 PONGTextColor = Vector3(1.0f, 0.4f, 0.2f);

	RenderList* renderList = nullptr;

	// Function to disable the menu.
	void DisableMenu();

public:
	PongMenuManager(Transform2D transform)
	{
		this->objectTransform = transform;
	}

	// Function to assign the play button object.
	void AssignPlayButtonObject(PlayButtonObject* playButtonObject);
	// Assign the game manager.
	void AssignGameManager(PongGameManager* gameManager);

	// Function to initialize the game object.
	void Initialize(RenderList* sceneRenderComponentList, PhysicsComponentList* physicsComponentList, AudioComponentList* audioComponentList, GUIComponentList* guiComponentList);
	// Function call for updating the game object state.
	void Update();

	// Function to enable the menu.
	void EnableMenu();
};