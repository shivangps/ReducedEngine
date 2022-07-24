#include "PongBall.h"
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

	this->renderComponent.ChangeColor(this->color);

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
}

void PongBall::Update()
{
	this->rigidbody->SetVelocity(this->direction.Normalize() * this->speed);
}

void PongBall::OnCollisionEnter2D(CollisionCharacteristics2D collision)
{
	if (collision.otherCollidingObject->CompareTag("PaddlePaddle"))
	{
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
		Vector2 reflectiveDirection = this->direction.Reflect(Vector2(1.0f, 0.0f).Normalize()).Normalize();		// Vertical paddle bounce.
		this->direction = reflectiveDirection;
	}
	if (collision.otherCollidingObject->CompareTag("Border"))
	{
		this->direction = this->direction.Reflect(Vector2(0.0f, 1.0f).Normalize());		// Horizontal border bounce.
	}
	if (collision.otherCollidingObject->CompareTag("ScoreBorder"))
	{

	}
}
