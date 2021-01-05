#pragma once

#include "Helper.h"
#include "MathEngine.h"

// Definition of basic game object present in the game scene.
class GameObject
{
private:
	Transform objectTransform;

public:
	GameObject() {}

	virtual void Update() {}

	// Get/Set functions.
	Transform GetTransform() { return this->objectTransform; }
	void SetTransform(Transform transform) { this->objectTransform = transform; }
};