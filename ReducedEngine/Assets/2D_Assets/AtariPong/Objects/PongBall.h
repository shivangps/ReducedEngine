#pragma once
#include "../GameObject2D.h"
#include "../Assets/2D_Assets/RenderComponents/WireframeRenderComponent2D.h"
#include "../Assets/2D_Assets/RenderComponents/SpriteRenderComponent2D.h"
#include "../Physics/2D/Rigidbody2D.h"
#include "../Physics/2D/Colliders/BoxCollider2D.h"
#include "../Output/Audio/AudioComponent.h"

// This class is the defination of the pong ball for bounce movement.
class PongBall : public GameObject2D
{
private:
	SpriteRenderComponent2D renderComponent = SpriteRenderComponent2D(&this->objectTransform);
	Rigidbody2D* rigidbody = nullptr;
	BoxCollider2D boxCollider = BoxCollider2D(&this->objectTransform);
	AudioComponent beepSound = AudioComponent();

	// Render characteristics.
	Vector3 color = Vector3((float) 255/255, (float) 136/255, (float) 0/255);

	// Physical chracteristics.
	float speed = 20.0f;
	Vector2 direction = Vector2(1.0f, 1.0f);
	bool toMove = true;

	// Scale of the pong ball.
	Vector2 scale = Vector2(0.5f);

	// Reset position.
	Vector2 resetPosition = Vector2(0.0f);

	AudioComponent* boundarySound = new AudioComponent();
	AudioComponent* paddleSound = new AudioComponent();
	AudioComponent* scoreSound = new AudioComponent();

	class PongGameManager* gameManager = nullptr;

	bool outOfBounds = false;

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

	// Function to reset the pong ball to a position.
	void ResetPosition();
	// Function to halt or move the ball.
	void SetMovement(bool toMove);

	// Function to get if the ball is out of bounds.
	bool IsOutofBounds();

	// Function to set the game manager.
	void SetGameManager(PongGameManager* gameManager);
};