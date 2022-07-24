#include "ScoreBorder.h"

void ScoreBorder::SetScore(int* ScorePointer)
{
	this->pScore = ScorePointer;
}

void ScoreBorder::Initialize(RenderList* sceneRenderComponentList, PhysicsComponentList* physicsComponentList, AudioComponentList* audioComponentList, GUIComponentList* guiComponentList)
{
	this->tag = "ScoreBorder";

	this->objectTransform.SetScale(1.0f, this->size);

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

	this->renderer.Load2DGeometry(vertices, indices);

	this->renderer.ChangeColor(Vector3(0.0f, 1.0f, 0.0f));

	sceneRenderComponentList->AssignRenderComponent2D(&this->renderer);

	// Setting physical cahracteristics.
	this->boxCollider = BoxCollider2D(&this->objectTransform);
	this->boxCollider.SetAsTrigger(true);
	this->rigidbody = new Rigidbody2D(&this->objectTransform, this, true, RigibodyType::RigibodyTypeStatic, true);
	this->rigidbody->AddColliderToRigidbody(&this->boxCollider);
	this->rigidbody->SetPhysicalCharacteristics(this);
	this->rigidbody->SetGravityScale(0.0f);
	this->rigidbody->SetMass(0.1f);

	physicsComponentList->RegisterRigidbody2D(this->rigidbody);
}

void ScoreBorder::OnCollisionEnter2D(CollisionCharacteristics2D collision)
{
	// Increase the score if it hits the pong ball.
	if (collision.otherCollidingObject->CompareTag("PongBall"))
	{
		*this->pScore += this->ScoreIncreament;
	}
}
