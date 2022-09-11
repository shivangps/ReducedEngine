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
}

void PlayerPaddle::MovePaddleUsingMouse()
{
	Vector2 pointerData = *this->mouse->GetMouseWindowPosition();
	float YPosition = this->ScreenTo3DCoordinates(pointerData).Y();

	if (YPosition > (this->characteristics.GetMedianVertical() + this->characteristics.GetMaxVerticalDistance()))
	{
		YPosition = this->characteristics.GetMedianVertical() + this->characteristics.GetMaxVerticalDistance();
	}
	if (YPosition < (this->characteristics.GetMedianVertical() - this->characteristics.GetMaxVerticalDistance()))
	{
		YPosition = this->characteristics.GetMedianVertical() - this->characteristics.GetMaxVerticalDistance();
	}

	this->objectTransform.SetPositionY(YPosition);
	this->rigidbody->ChangeTransform(this->objectTransform);
}

Vector2 PlayerPaddle::ScreenTo3DCoordinates(Vector2 ScreenCoordinates)
{
	Vector2 screenResolution(output->GetWindowWidth(), output->GetWindowHeight());
	Vector2 result;
	Camera2D camera = MainCamera2D::GetInstance()->GetCamera2D();
	
	Vector2 screenRatio = Vector2(ScreenCoordinates.X() / screenResolution.X(), ScreenCoordinates.Y() / screenResolution.Y());
	result = screenRatio * Vector2(camera.width, camera.height);
	result = result - Vector2(camera.width / 2, camera.height / 2);
	result.Y(-result.Y());

	return result;
}

void PlayerPaddle::Initialize(RenderList* sceneRenderComponentList, PhysicsComponentList* physicsComponentList, AudioComponentList* audioComponentList, GUIComponentList* guiComponentList)
{
	this->tag = "PaddlePaddle";
	// Set the X axis position of the paddle.
	this->objectTransform.SetScale(this->characteristics.GetScale());
	this->objectTransform.SetPositionX(this->characteristics.GetXValue());

	// Sprite Render Component initialization.
	SpriteRenderComponent2D* newSpriteRenderer = new SpriteRenderComponent2D(&this->objectTransform);

	std::vector<MeshVertex2D> vertices = this->characteristics.GetVertices();
	std::vector<unsigned short> indices = this->characteristics.GetIndices();
	newSpriteRenderer->Load2DGeometry(vertices, indices);

	newSpriteRenderer->ChangeColor(this->color);

	newSpriteRenderer->LoadSpriteTexture(this->characteristics.GetPaddleTexture());

	newSpriteRenderer->SetGameObject(this);

	this->spriteRenderer = newSpriteRenderer;

	sceneRenderComponentList->AssignRenderComponent2D(this->spriteRenderer);

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
	if (abs(controller->GetRightThumbNormalizedX()) > 0.1f || abs(controller->GetRightThumbNormalizedY()) > 0.1f)
	{
		this->MovePaddleUsingController();
	}
	else
	{
		this->MovePaddleUsingMouse();
	}
}
