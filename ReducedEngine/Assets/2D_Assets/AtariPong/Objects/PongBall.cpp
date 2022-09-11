#include "PongBall.h"
#include "PongGameManager.h"
#include "PongPaddle.h"
#include <random>

void PongBall::Initialize(RenderList* sceneRenderComponentList, PhysicsComponentList* physicsComponentList, AudioComponentList* audioComponentList, GUIComponentList* guiComponentList)
{
	this->tag = "PongBall";

	this->objectTransform.SetScale(this->scale);

	DebugLog(std::to_string(DirectX::XMConvertToDegrees(atan2(this->direction.Y(), this->direction.X()))) + "\n");

	// Render component initialization.
	std::vector<MeshVertex2D> vertices;
	vertices.push_back(MeshVertex2D(-0.5f, 0.5f, 0.0f, 1.0f));
	vertices.push_back(MeshVertex2D(-0.5f, -0.5f, 0.0f, 0.0f));
	vertices.push_back(MeshVertex2D(0.5f, 0.5f, 1.0f, 1.0f));
	vertices.push_back(MeshVertex2D(0.5f, -0.5f, 1.0f, 0.0f));

	std::vector<unsigned short> indices;
	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);

	indices.push_back(1);
	indices.push_back(2);
	indices.push_back(3);

	this->renderComponent.Load2DGeometry(vertices, indices);

	this->renderComponent.LoadSpriteTexture("Assets/Textures/PongBall.png");

	this->renderComponent.ChangeColor(this->color);

	this->renderComponent.SetGameObject(this);

	sceneRenderComponentList->AssignRenderComponent2D(&this->renderComponent);

	// Physics components initilazation.
	this->boxCollider = BoxCollider2D(&this->objectTransform);
	this->boxCollider.SetAsTrigger(true);
	this->rigidbody = new Rigidbody2D(&this->objectTransform, this, false, RigibodyType::RigibodyTypeDynamic, true);
	this->rigidbody->AddColliderToRigidbody(&this->boxCollider);
	this->rigidbody->SetPhysicalCharacteristics(this);
	this->rigidbody->SetGravityScale(0.0f);
	this->rigidbody->SetMass(0.1f);

	physicsComponentList->RegisterRigidbody2D(this->rigidbody);

	// Initializing sound.
	this->boundarySound->Initialize("Assets/SoundSamples/BoundarySound.mp3");
	audioComponentList->AssignAudioComponent(this->boundarySound);
	this->paddleSound->Initialize("Assets/SoundSamples/PaddleSound.mp3");
	audioComponentList->AssignAudioComponent(this->paddleSound);
	this->scoreSound->Initialize("Assets/SoundSamples/ScoreSound.mp3");
	audioComponentList->AssignAudioComponent(this->scoreSound);

}

void PongBall::Update()
{
	if (this->toMove)
	{
		this->rigidbody->SetVelocity(this->direction.Normalize() * this->speed);
	}
	else
	{
		this->rigidbody->SetVelocity(Vector2(0.0f));
	}

	{
		// Calculate if out of bounds.
		Vector2 position = this->objectTransform.GetGlobalPostion();
		PaddleCharacteristics characteristics = PaddleCharacteristics();

		float buffer = 4.0f;
		float minX = characteristics.GetMedianVertical() - characteristics.GetMaxHorizontalDistance() - buffer;
		float maxX = characteristics.GetMedianVertical() + characteristics.GetMaxHorizontalDistance() + buffer;
		float minY = characteristics.GetMedianVertical() - characteristics.GetMaxVerticalDistance() - buffer;
		float maxY = characteristics.GetMedianVertical() + characteristics.GetMaxVerticalDistance() + buffer;

		if (position.X() > maxX || position.X() < minX || position.Y() > maxY || position.Y() < minY)
		{
			this->outOfBounds = true;
		}
		else
		{
			this->outOfBounds = false;
		}
	}
}

void PongBall::OnCollisionEnter2D(CollisionCharacteristics2D collision)
{
	if (collision.otherCollidingObject->CompareTag("PaddlePaddle"))
	{
		this->paddleSound->Play();

		float maxRange = 10.0f;
		float minRange = -10.0f;

		Vector2 reflectiveDirection = this->direction.Reflect(Vector2(1.0f, 0.0f).Normalize()).Normalize();		// Vertical paddle bounce.
		float angle = DirectX::XMConvertToDegrees(atan2(reflectiveDirection.Y(), reflectiveDirection.X()));
		float random = ((float)rand() / (float)RAND_MAX);
		float diff = maxRange - minRange;
		float r = random * diff;
		float newAngle = angle + minRange + r;
		DebugLog("Angle: " +  std::to_string(angle) + "\n");
		newAngle = DirectX::XMConvertToRadians(newAngle);
		reflectiveDirection = Vector2(cosf(newAngle), sinf(newAngle));
		this->direction = reflectiveDirection;
	}
	if (collision.otherCollidingObject->CompareTag("EnemyPaddle"))
	{
		this->paddleSound->Play();

		Vector2 reflectiveDirection = this->direction.Reflect(Vector2(1.0f, 0.0f).Normalize()).Normalize();		// Vertical paddle bounce.
		this->direction = reflectiveDirection;
	}
	if (collision.otherCollidingObject->CompareTag("Border"))
	{
		this->boundarySound->Play();
		this->direction = this->direction.Reflect(Vector2(0.0f, 1.0f).Normalize());		// Horizontal border bounce.
	}
	if (collision.otherCollidingObject->CompareTag("ScoreBorder"))
	{
		this->scoreSound->Play();
	}
}

void PongBall::ResetPosition()
{
	this->rigidbody->SetToPosition(this->resetPosition);
}

void PongBall::SetMovement(bool toMove)
{
	this->toMove = toMove;
}

bool PongBall::IsOutofBounds()
{
	return this->outOfBounds;
}

void PongBall::SetGameManager(PongGameManager* gameManager)
{
	this->gameManager = gameManager;
}
