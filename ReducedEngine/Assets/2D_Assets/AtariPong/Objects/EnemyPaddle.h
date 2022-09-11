#pragma once
#include "PongPaddle.h"

class PongBall;
// CLass that uses AI to play against player.
class EnemyPaddle : public GameObject2D
{
private:
	SpriteRenderComponent2D* spriteRenderer = nullptr;
	BoxCollider2D boxCollider = BoxCollider2D(&this->objectTransform);
	Rigidbody2D* rigidbody = nullptr;

	XController* controller = Input::GetInstance()->GetController();
	Time* time = Time::GetInstance();

	PaddleCharacteristics characteristics;

	// Object color to red to represent player.
	Vector3 color = Vector3((float) 255/255, (float) 0/255, (float) 68/255);

	// Pong ball pointer to move the paddle based on the pong ball position.
	PongBall* pongBall;

	// Function that is responsible for movement of the enemy paddle.
	void MovementAlgorithm();

public:
	EnemyPaddle(Transform2D transform)
	{
		this->objectTransform = transform;
	}

	// Function to initialize the game object.
	void Initialize(RenderList* sceneRenderComponentList, PhysicsComponentList* physicsComponentList, AudioComponentList* audioComponentList, GUIComponentList* guiComponentList);
	// Function that is called every frame.
	void Update();
	// Function to assign the current pong ball present in the scene.
	void AssignPongBall(PongBall* pongBall);
};