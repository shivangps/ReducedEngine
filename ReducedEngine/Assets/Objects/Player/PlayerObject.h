#pragma once
#include "../GameObject.h"

class PlayerObject : public GameObject
{
private:
	Camera camera = { &this->objectTransform };

public:
	// Speed for the camera to rotate using mouse.
	float camera_rotation_speed = 0.03f;
	// Speed for the camera to move using keys.
	float camera_move_speed = 0.1f;

	PlayerObject();

	void Update();
};