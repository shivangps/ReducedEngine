#pragma once
#include "../../../../GameObject2D.h"
#include "../../../../Output/Graphics/2D/RenderComponent2D.h"
#include "../../../../Physics/2D/Colliders/BoxCollider2D.h"
#include "../../../../Physics/2D/Rigidbody2D.h"

// This game object defines the box object.

class PlatformObject2D : public GameObject2D
{
private:
	RenderComponent2D* wireframeRenderer = nullptr;
	BoxCollider2D* boxCollider = nullptr;
	Rigidbody2D* rigidbody = nullptr;

	// Object color to green.
	Vector3 color = Vector3(0.0f, 0.7f, 0.0f);

public:
	PlatformObject2D() {}
	PlatformObject2D(Transform2D transform);

	// Movement speed of the box.
	float key_movement_speed = 0.1f;
	float controller_movement_speed = 0.01;

	// Function to initialize the game object.
	void Initialize(RenderList* sceneRenderComponentList, PhysicsComponentList* physicsComponentList, AudioComponentList* audioComponentList, GUIComponentList* guiComponentList);
	void Update();
};