#pragma once

#include "Output/Graphics/RenderList.h"
#include "../Physics/PhysicsComponentList.h"
#include <box2d/b2_settings.h>

// Definition of basic game object present in the game scene.
class GameObject2D : public PhysicalCharacteristics2D
{
protected:
	bool isStatic = false;

	Transform2D objectTransform;

public:
	GameObject2D() {}

	// Function to initialize the game object.
	virtual void Initialize(RenderList* sceneRenderComponentList, PhysicsComponentList* physicsComponentList) {}
	// Function call for updating the game object state.
	virtual void Update() {}

	// Function to get transform of the gameobject.
	Transform2D* GetTransform2D() { return &this->objectTransform; }
	// Function to set the object static or dynamic.
	virtual void SetAsStatic();
	virtual void SetAsDynamic();
	bool GetStaticState();
};