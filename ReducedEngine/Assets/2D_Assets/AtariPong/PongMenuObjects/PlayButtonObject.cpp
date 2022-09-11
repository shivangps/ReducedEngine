#include "PlayButtonObject.h"
#include "../Assets/2D_Assets/RenderComponents/SpriteRenderComponent2D.h"

Vector2 PlayButtonObject::ScreenToCoordinates(Vector2 ScreenCoordinates)
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

void PlayButtonObject::Initialize(RenderList* sceneRenderComponentList, PhysicsComponentList* physicsComponentList, AudioComponentList* audioComponentList, GUIComponentList* guiComponentList)
{
	// Assigning statics.
	this->mouse = Input::GetInstance()->GetMouse();

	// Initializing render component for button.
	this->renderer = new SpriteRenderComponent2D(&this->objectTransform);

	// Vertices.
	std::vector<MeshVertex2D> vertices = {};

	vertices.push_back(MeshVertex2D(-0.5f, 0.5f, 0.0f, 0.0f));
	vertices.push_back(MeshVertex2D(-0.5f, -0.5f, 0.0f, 1.0f));
	vertices.push_back(MeshVertex2D(0.5f, 0.5f, 1.0f, 0.0f));
	vertices.push_back(MeshVertex2D(0.5f, -0.5f, 1.0f, 1.0f));

	// Indices.
	std::vector<unsigned short> indices;

	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);

	indices.push_back(1);
	indices.push_back(2);
	indices.push_back(3);

	this->renderer->Load2DGeometry(vertices, indices);
	this->renderer->LoadSpriteTexture("Assets/Textures/MenuButton.png");
	this->renderer->ChangeColor(this->buttonColor);
	this->renderer->SetGameObject(this);

	sceneRenderComponentList->AssignRenderComponent2D(this->renderer);

	// Initializing the text sprite.
	this->textRenderer = new SpriteRenderComponent2D(&this->objectTransform);
	this->textRenderer->Load2DGeometry(vertices, indices);
	this->textRenderer->LoadSpriteTexture("Assets/Textures/PlayText.png");
	this->textRenderer->ChangeColor(this->textColor);
	this->textRenderer->SetGameObject(this);

	sceneRenderComponentList->AssignRenderComponent2D(this->textRenderer);

	// Initializing transforms.
	// Default.
	Vector2 originalScale = Vector2(3.0f, 1.0f);
	this->defaultState.SetPosition(-1.0f, 0.0f);
	this->defaultState.SetScale(originalScale * 3.0f);

	// Hover.
	this->hoverState.SetPosition(-1.0f, 0.0f);
	this->hoverState.SetScale(originalScale * 2.8f);

	// Held.
	this->heldState.SetPosition(-1.0f, 0.0f);
	this->heldState.SetScale(originalScale * 2.5f);

	// Assign State.
	this->objectTransform = this->defaultState;
}

void PlayButtonObject::Update()
{
	// Get mouse position.
	this->mouse->ShowMouseCursor(true);

	Vector2 pointerData = *this->mouse->GetMouseWindowPosition();
	pointerData = this->ScreenToCoordinates(pointerData);
	
	float maxX = this->objectTransform.GetGlobalPostion().X() + (this->objectTransform.GetGlobalScale().X() / 2);
	float minX = this->objectTransform.GetGlobalPostion().X() - (this->objectTransform.GetGlobalScale().X() / 2);
	float maxY = this->objectTransform.GetGlobalPostion().Y() + (this->objectTransform.GetGlobalScale().Y() / 2);
	float minY = this->objectTransform.GetGlobalPostion().Y() - (this->objectTransform.GetGlobalScale().Y() / 2);

	this->isClicked = false;

	// Check if the pointer is in boundary of the button.
	if (pointerData.X() < maxX && pointerData.X() > minX && pointerData.Y() < maxY && pointerData.Y() > minY)
	{
		if (this->mouse->IsLeftDown())
		{
			this->isClicked = true;
			this->objectTransform = this->heldState;
		}
		else
		{
			this->objectTransform = this->hoverState;
		}
	}
	else
	{
		this->objectTransform = this->defaultState;
	}

	// Clicking with Xbox controller.
	XInput_ButtonID button_id;
	if (controller->GetButtonDown(button_id.A))
	{
		this->isClicked = true;
	}
}

bool PlayButtonObject::GetClickState()
{
	return this->isClicked;
}
