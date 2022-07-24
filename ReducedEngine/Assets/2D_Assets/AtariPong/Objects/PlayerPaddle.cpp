#include "PlayerPaddle.h"

void PlayerPaddle::MovePaddleUsingController()
{
	XInput_ButtonID button_id;
	float delta_time = this->time->GetDeltaTime();

	// Using the right thumbstick Y axis value to traverse the paddle.
	float thumbstickValue = this->controller->GetRightThumbNormalizedY();

	bool isThumbstickInDeadzone = this->controller->IsRightThumbDeadzone();

	float paddleYValue = 0.0f;

	paddleYValue = this->characteristics.GetMedianVertical() + (thumbstickValue * this->characteristics.GetMaxVerticalDistance());

	Transform2D newTransform = this->objectTransform;
	newTransform.SetPositionY(paddleYValue);
	this->rigidbody->ChangeTransform(newTransform);

	// Quit pressing B button.
	if (this->controller->GetButtonDown(button_id.B))
	{
		Output::GetInstance()->CloseWindow();
	}
}

void PlayerPaddle::Initialize(RenderList* sceneRenderComponentList, PhysicsComponentList* physicsComponentList, AudioComponentList* audioComponentList, GUIComponentList* guiComponentList)
{
	this->tag = "PaddlePaddle";
	// Set the x axis position of the paddle.
	this->objectTransform.SetScale(this->characteristics.GetScale());
	this->objectTransform.SetPositionX(this->characteristics.GetXValue());

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

void PlayerPaddle::Update()
{
	this->MovePaddleUsingController();
}
