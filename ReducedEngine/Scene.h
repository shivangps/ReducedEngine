#pragma once

#include "GameObject.h"

// Scene contains collection of game objects present in the scene.
class Scene
{
protected:
	// Collections of game objects present in the scene.
	std::vector<GameObject*> objectsPresent = {};

	// Collection of render components.
	RenderList renderComponentList = {};

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
};