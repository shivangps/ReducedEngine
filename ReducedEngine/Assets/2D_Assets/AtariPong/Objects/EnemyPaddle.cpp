#include "EnemyPaddle.h"
#include "PongBall.h"

void EnemyPaddle::MovementAlgorithm()
{
	// Move wherever the pong ball is moving. (Simple A.I.)
	if (this->pongBall)
	{
		float pongBallY = this->pongBall->GetTransform2D()->GetGlobalPostion().Y();
		float paddleY = this->objectTransform.GetGlobalPostion().Y();
		float YDifference = pongBallY - paddleY;
		float newYPosition = paddleY + YDifference;
		Transform2D newTransform = this->objectTransform;
		if (newYPosition > (this->characteristics.GetMedianVertical() + this->characteristics.GetMaxVerticalDistance()))
		{
			newTransform.SetPositionY(this->characteristics.GetMedianVertical() + this->characteristics.GetMaxVerticalDistance());
		}
		else
		{
			newTransform.SetPositionY(newYPosition);
		}
		if(newYPosition < (this->characteristics.GetMedianVertical() - this->characteristics.GetMaxVerticalDistance()))
		{
			newTransform.SetPositionY(this->characteristics.GetMedianVertical() - this->characteristics.GetMaxVerticalDistance());
		}
		else
		{
			newTransform.SetPositionY(newYPosition);
		}
		this->rigidbody->ChangeTransform(newTransform);
	}
}

void EnemyPaddle::Initialize(RenderList* sceneRenderComponentList, PhysicsComponentList* physicsComponentList, AudioComponentList* audioComponentList, GUIComponentList* guiComponentList)
{
	this->tag = "EnemyPaddle";
	// Set the x axis position of the paddle.
	this->objectTransform.SetScale(this->characteristics.GetScale());
	this->objectTransform.SetPositionX(-this->characteristics.GetXValue());

	WireframeRenderComponent2D* newRenderComponent2D = new WireframeRenderComponent2D(&this->objectTransform);

	newRenderComponent2D->Load2DGeometry(characteristics.GetVertices(), characteristics.GetIndices());

	newRenderComponent2D->ChangeColor(this->color);

	this->renderer = newRenderComponent2D;

	sceneRenderComponentList->AssignRenderComponent2D(this->renderer);

	// Setting physical cahracteristics.
	this->boxCollider = BoxCollider2D(&this->objectTransform);
	this->boxCollider.SetAsTrigger(true);
	this->rigidbody = new Rigidbody2D(&this->objectTransform, this, false, RigibodyType::RigibodyTypeKinematic, true);
	this->rigidbody->AddColliderToRigidbody(&this->boxCollider);
	this->rigidbody->SetPhysicalCharacteristics(this);
	this->rigidbody->SetGravityScale(0.0f);
	this->rigidbody->SetMass(0.1f);

	physicsComponentList->RegisterRigidbody2D(this->rigidbody);
}

void EnemyPaddle::Update()
{
	this->MovementAlgorithm();
}

void EnemyPaddle::AssignPongBall(PongBall* pongBall)
{
	this->pongBall = pongBall;
}
