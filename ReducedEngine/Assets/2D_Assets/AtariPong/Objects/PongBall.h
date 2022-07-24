#pragma once
#include "../GameObject2D.h"
#include "../Assets/2D_Assets/RenderComponents/WireframeRenderComponent2D.h"
#include "../Physics/2D/Rigidbody2D.h"
#include "../Physics/2D/Colliders/BoxCollider2D.h"
#include "../Output/Audio/AudioComponent.h"

// This class is the defination of the pong ball for bounce movement.
class PongBall : public GameObject2D
{
private:
	WireframeRenderComponent2D renderComponent = WireframeRenderComponent2D(&this->objectTransform);
	Rigidbody2D* rigidbody = nullptr;
	BoxCollider2D boxCollider = BoxCollider2D(&this->objectTransform);
	AudioComponent beepSound = AudioComponent();

	// Render characteristics.
	Vector3 color = Vector3(1.0f);

	// Physical chracteristics.
	float speed = 10.0f;
	Vector2 direction = Vector2(1.0f, 1.0f);

	// Scale of the pong ball.
	Vector2 scale = Vector2(0.5f);

public:
	PongBall(Transform2D transform)
	{
		this->objectTransform = transform;
	}

	// Function to initialize the game object.
	void Initialize(RenderList* sceneRenderComponentList, PhysicsComponentList* physicsComponentList, AudioComponentList* audioComponentList, GUIComponentList* guiComponentList);
	// Function call for updating the game object state.
	void Update();

	// Callback function for the physical collisions.
	void OnCollisionEnter2D(CollisionCharacteristics2D collision);
};