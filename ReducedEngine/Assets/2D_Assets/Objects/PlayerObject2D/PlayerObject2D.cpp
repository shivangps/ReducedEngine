#include "PlayerObject2D.h"
#include "../Assets/2D_Assets/RenderComponents/WireframeRenderComponent2D.h"

void PlayerObject2D::MoveBoxUsingKeys()
{
	float deltaTime = this->time->GetDeltaTime();

	// Move left.
	if (keyboard->KeyIsPressed('A'))
	{
		//this->objectTransform.Translate(this->objectTransform.GetLocalLeft() * this->key_movement_speed * deltaTime);
		this->rigidbody->TranslateBody(this->objectTransform.GetLocalLeft() * this->key_movement_speed);
	}
	// Move right.
	if (keyboard->KeyIsPressed('D'))
	{
		//this->objectTransform.Translate(this->objectTransform.GetLocalRight() * this->key_movement_speed * deltaTime);
		this->rigidbody->TranslateBody(this->objectTransform.GetLocalRight() * this->key_movement_speed);
	}
	// Jump.
	if (keyboard->IsSpacePressed() && !this->onAir)
	{
		this->rigidbody->TranslateBody(this->objectTransform.GetLocalUp() * this->jumpForce);
	}
}

void PlayerObject2D::MoveBoxUsingController()
{
	XInput_ButtonID button_id = {};
	float deltaTime = this->time->GetDeltaTime();

	// Movement.
	// D-Pad.
	if (this->controller->GetButtonPressed(button_id.Dpad_Left))
	{
		//this->objectTransform.Translate(this->objectTransform.GetLocalLeft() * this->controller_movement_speed * deltaTime);
		this->rigidbody->TranslateBody(this->objectTransform.GetLocalLeft() * this->controller_movement_force);
	}
	if (this->controller->GetButtonPressed(button_id.Dpad_Right))
	{
		//this->objectTransform.Translate(this->objectTransform.GetLocalRight() * this->controller_movement_speed * deltaTime);
		this->rigidbody->TranslateBody(this->objectTransform.GetLocalRight() * this->controller_movement_force);
	}
	// Left-Thumbstick.
	if (this->controller->GetLeftThumbNormalizedMagnitude() > this->leftThumbstickDeadzone)
	{
		this->rigidbody->TranslateBody(this->objectTransform.GetLocalRight() * this->controller->GetLeftThumbNormalizedX() * this->controller_movement_force);
	}
	// Press A button to jump.
	if (this->controller->GetButtonPressed(button_id.A) && !this->onAir)
	{
		this->rigidbody->TranslateBody(this->objectTransform.GetLocalUp() * this->jumpForce);
	}

	// Quit pressing B button.
	if (this->controller->GetButtonDown(button_id.B))
	{
		Output::GetInstance()->CloseWindow();
	}
}

void PlayerObject2D::EnableRumble()
{
	this->output->SetLeftRumble(this->controller->GetPadID(), 1.0f);
	this->rumbleBool = true;
}

void PlayerObject2D::DisableRumble()
{
	this->output->SetLeftRumble(this->controller->GetPadID(), 0.0f);
	this->rumbleBool = false;
}

PlayerObject2D::PlayerObject2D(Transform2D transform)
{
	this->objectTransform = transform;
}

void PlayerObject2D::Initialize(RenderList* sceneRenderComponentList, PhysicsComponentList* physicsComponentList, AudioComponentList* audioComponentList, GUIComponentList* guiComponentList)
{
	WireframeRenderComponent2D* newRenderComponent2D = new WireframeRenderComponent2D(&this->objectTransform);

	std::vector<MeshVertex2D> vertices;
	vertices.push_back(MeshVertex2D(-0.5f, 0.5f, 0.0f, 1.0f));
	vertices.push_back(MeshVertex2D(-0.5f,-0.5f, 0.0f, 0.0f));
	vertices.push_back(MeshVertex2D( 0.5f, 0.5f, 1.0f, 1.0f));
	vertices.push_back(MeshVertex2D( 0.5f,-0.5f, 1.0f, 0.0f));

	std::vector<unsigned short> indices;
	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);

	indices.push_back(1);
	indices.push_back(2);
	indices.push_back(3);

	newRenderComponent2D->Load2DGeometry(vertices, indices);

	newRenderComponent2D->ChangeColor(this->color);

	this->wireframeRenderer = newRenderComponent2D;

	sceneRenderComponentList->AssignRenderComponent2D(this->wireframeRenderer);

	// Physics.
	this->boxCollider = new BoxCollider2D(&this->objectTransform);

	this->rigidbody = new Rigidbody2D(&this->objectTransform, this, this->isStatic, RigibodyType::RigibodyTypeDynamic, true);
	this->rigidbody->AddColliderToRigidbody(this->boxCollider);
	this->rigidbody->SetPhysicalCharacteristics(this);
	this->rigidbody->SetMass(5.0f);
	physicsComponentList->RegisterRigidbody2D(this->rigidbody);

	// Audio and sounds.

	// Jumping.
	this->jumpSound = new AudioComponent();
	this->jumpSound->Initialize(this->jumpSoundFile);
	this->jumpSound->SetVolume(this->jumpSoundVolume);

	// Landing on jump.
	this->landedSound = new AudioComponent();
	this->landedSound->Initialize(this->landedSoundFile);
	this->landedSound->SetVolume(this->landedSoundVolume);

	audioComponentList->AssignAudioComponent(this->jumpSound);
	audioComponentList->AssignAudioComponent(this->landedSound);
}

void PlayerObject2D::Update()
{
	this->MoveBoxUsingKeys();
	this->MoveBoxUsingController();

	// Rumble the controller for set duration and then disable it.
	if (this->rumbleBool && this->timeKeepForRumble < this->rumbleDuration)
	{
		this->timeKeepForRumble += (this->time->GetDeltaTime() / 1000.0f);
		this->EnableRumble();
	}
	else
	{
		this->DisableRumble();
		this->timeKeepForRumble = 0.0f;
	}
}

void PlayerObject2D::OnCollisionEnter2D()
{
	this->onAir = false;
	if (!this->rumbleBool)
	{
		this->EnableRumble();
	}
	this->color = Vector3(0.0f, 0.0f, 1.0f);
	this->wireframeRenderer->ChangeColor(this->color);
	this->landedSound->Play();
}

void PlayerObject2D::OnCollisionExit2D()
{
	this->onAir = true;
	this->color = Vector3(0.0f, 1.0f, 1.0f);
	this->wireframeRenderer->ChangeColor(this->color);
	this->jumpSound->Play();
}
