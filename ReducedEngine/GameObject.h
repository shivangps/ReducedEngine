#pragma once

#include "Output/Graphics/RenderList.h"

// Definition of basic game object present in the game scene.
class GameObject
{
protected:
	Transform objectTransform;

public:
	GameObject() {}

	// Function to initialize the game object.
	virtual void Initialize(RenderList* sceneRenderComponentList) {}
	// Function call for updating the game object state.
	virtual void Update() {}

	// Function to get transform of the gameobject.
	Transform GetTransform() { return this->objectTransform; }
};