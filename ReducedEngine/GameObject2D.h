#pragma once

#include "Output/Graphics/RenderList.h"
#include "../Physics/PhysicsComponentList.h"
#include "Output/Audio/AudioComponentList.h"
#include "Output/Graphics/GUI/GUIComponentList.h"
#include <box2d/b2_settings.h>

// Definition of basic game object present in the game scene.
class GameObject2D : public PhysicalCharacteristics2D
{
protected:
	bool isStatic = false;

	Transform2D objectTransform;

	std::string tag = "Empty Tag";

	class Scene* associatedScene = nullptr;

	bool isEnabled = true;

public:
	GameObject2D() {}

	// Function to initialize the game object.
	virtual void Initialize(RenderList* sceneRenderComponentList, PhysicsComponentList* physicsComponentList, AudioComponentList* audioComponentList, GUIComponentList* guiComponentList) {}
	// Function call for updating the game object state.
	virtual void Update() {}

	// Function to get transform of the gameobject.
	Transform2D* GetTransform2D() { return &this->objectTransform; }
	// Function to set the object static or dynamic.
	virtual void SetAsStatic();
	virtual void SetAsDynamic();
	bool GetStaticState();

	// Fucntion to compare the tag of the game object and its the same or not.
	bool CompareTag(std::string otherTag);

	// Function to set the scene of the game object.
	void SetScene(Scene* scene);

	// Get the enabled state of the object.
	bool GetEnabledState();
	// Set the enabled state.
	void EnableGameObject();
	void DisableGameObject();
};