#pragma once
#include "../GameObject2D.h"
#include "../Assets/2D_Assets/RenderComponents/WireframeRenderComponent2D.h"
#include "../Physics/2D/Colliders/BoxCollider2D.h"
#include "../Physics/2D/Rigidbody2D.h"

class BorderObject : public GameObject2D
{
private:
	WireframeRenderComponent2D renderComponent = WireframeRenderComponent2D(&this->objectTransform);
	Rigidbody2D* rigidbody = nullptr;
	BoxCollider2D boxCollider = BoxCollider2D(&this->objectTransform);

	// Set the boundary transform parameters.
	Vector2 size = Vector2(40.0f, 1.0f);

	// To render the border for debugging.
	bool toRender = false;

public:
	BorderObject(Transform2D transform)
	{
		this->objectTransform = transform;
	}

	// Function to initialize the game object.
	void Initialize(RenderList* sceneRenderComponentList, PhysicsComponentList* physicsComponentList, AudioComponentList* audioComponentList, GUIComponentList* guiComponentList);
	// Function call for updating the game object state.
	void Update();
};