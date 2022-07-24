#pragma once
#include "../GameObject2D.h"
#include "../../RenderComponents/WireframeRenderComponent2D.h"
#include "../Physics/2D/Colliders/BoxCollider2D.h"
#include "PongGameManager.h"

// Game object that if collided with the pong ball increases the score.
class ScoreBorder : public GameObject2D
{
private:
	WireframeRenderComponent2D renderer = WireframeRenderComponent2D(&this->objectTransform);
	BoxCollider2D boxCollider = BoxCollider2D(&this->objectTransform);
	Rigidbody2D* rigidbody = nullptr;

	// Size of the score border.
	float size = 20.0f;

	// Score increament.
	int ScoreIncreament = 1;

	// Pointer to the score variable.
	int* pScore = nullptr;

public:
	ScoreBorder(Transform2D transform)
	{
		this->objectTransform = transform;
	}

	// Function to set the pointer to score variable.
	void SetScore(int* ScorePointer);

	// Function to initialize the game object.
	void Initialize(RenderList* sceneRenderComponentList, PhysicsComponentList* physicsComponentList, AudioComponentList* audioComponentList, GUIComponentList* guiComponentList);

	// Callback function for the physical collisions.
	void OnCollisionEnter2D(CollisionCharacteristics2D collision);
};