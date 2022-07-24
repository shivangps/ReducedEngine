#pragma once
#include "../../../../GameObject2D.h"
#include "../../../../Output/Graphics/2D/RenderComponent2D.h"
#include "../Assets/2D_Assets/RenderComponents/WireframeRenderComponent2D.h"
#include "../../../../Physics/2D/Colliders/BoxCollider2D.h"
#include "../../../../Physics/2D/Rigidbody2D.h"

// Player object controlled by keyboard, mouse and controller.

class PlayerObject2D : public GameObject2D
{
private:
	WireframeRenderComponent2D* wireframeRenderer = nullptr;
	BoxCollider2D* boxCollider = nullptr;
	Rigidbody2D* rigidbody = nullptr;

	AudioComponent* jumpSound = nullptr;
	std::string jumpSoundFile = "Assets/SoundSamples/Woosh.mp3";
	float jumpSoundVolume = 0.1f;

	AudioComponent* landedSound = nullptr;
	std::string landedSoundFile = "Assets/SoundSamples/CardboardBox_Drop.wav";
	float landedSoundVolume = 0.3f;

	KeyboardClass* keyboard = Input::GetInstance()->GetKeyboard();
	XController* controller = Input::GetInstance()->GetController();
	Output* output = Output::GetInstance();
	Time* time = Time::GetInstance();

	// Object color to red to represent player.
	Vector3 color = Vector3(0.7f, 0.0f, 0.0f);

	bool onAir = false;
	bool rumbleBool = false;

	float rumbleDuration = 0.2f;
	float timeKeepForRumble = 0.0f;

	float leftThumbstickDeadzone = 0.1f;



	// Function to move the box using key input.
	void MoveBoxUsingKeys();
	// Function to move the box using xbox controller inputs.
	void MoveBoxUsingController();
	// Function to rumble the controller.
	void EnableRumble();
	void DisableRumble();

public:
	PlayerObject2D() {}
	PlayerObject2D(Transform2D transform);

	// Movement speed of the box.
	float key_movement_speed = 0.1f;
	float controller_movement_speed = 0.01;
	float controller_movement_force = 0.1f;
	float jumpForce = 2.0f;

	// Function to initialize the game object.
	void Initialize(RenderList* sceneRenderComponentList, PhysicsComponentList* physicsComponentList, AudioComponentList* audioComponentList, GUIComponentList* guiComponentList);
	// Function that is called every frame.
	void Update();

	// Function to call when the object is entering collision.
	void OnCollisionEnter2D();
	void OnCollisionExit2D();
};