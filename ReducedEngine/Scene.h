#pragma once

#include "GameObject.h"
#include "GameObject2D.h"

// Scene contains collection of game objects present in the scene.
class Scene
{
protected:
	// Collections of game objects present in the scene.
	std::vector<GameObject*> objectsPresent = {};
	std::vector<GameObject2D*> objects2DPresent = {};

	// Condition to change the scene.
	bool toChange = false;
	// The pointer to the scene to be changed.
	Scene* sceneToChangeTo = nullptr;

	// Collection of render components.
	RenderList renderComponentList = {};
	// Collection of rigidbody components.
	PhysicsComponentList physicsComponentList = {};
	// Collection of audio components.
	AudioComponentList audioComponentList = {};
	// Collection of GUI components.
	GUIComponentList guiComponentList = {};

public:
	Scene() {}

	// Fucntion to get size of collection.
	unsigned int GetSize();
	// Function to verify if index is going out of count.
	bool CheckIndexSanity(unsigned int index);
	// Function to get the game object in the scene.
	GameObject* GetGameObject(unsigned int index);
	// Function to set a new game object.
	void SetGameObject(GameObject* newGameObject);
	// Function to get the render component list.
	RenderList* GetRenderComponentList();
	// Function to get the physics component list.
	PhysicsComponentList* GetPhysicsComponentList();
	// Function to get the audio component list.
	AudioComponentList* GetAudioComponentList();
	// Function to get the GUI component list.
	GUIComponentList* GetGUIComponentList();

	unsigned int GetSize2D();
	// Function to get the game object in the scene.
	GameObject2D* GetGameObject2D(unsigned int index);
	// Function to verify if index is going out of count.
	bool CheckIndexSanity2D(unsigned int index);
	// Function to det a new 2D game object.
	void SetGameObject2D(GameObject2D* newGameObject2d);

	// Function to change to a new scene.
	void ChangeToScene(Scene* nextScene);
	// Function to get if the scene is to be changed.
	bool ToChangeScene();
	// Function to get the scene to be changed.
	Scene* GetChangedScene();
};