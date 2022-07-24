#include "MainScene2D.h"
#include "../Assets/2D_Assets/Objects/PlatformObject2D/PlatformObject2D.h"
#include "../Assets/2D_Assets/Objects/CameraObject2D/CameraObject2D.h"
#include "../Assets/2D_Assets/Objects/PlayerObject2D/PlayerObject2D.h"


// Setting the scene in 2D space.
MainScene2D::MainScene2D()
{
	GameObject2D* newGameobject2D = nullptr;
	Transform2D setTransform2d;

	// Platform Object 2D.
	setTransform2d = Transform2D();
	setTransform2d.SetScale(5.0f, 1.0f);
	PlatformObject2D* platformObject2D = new PlatformObject2D(setTransform2d);
	this->objects2DPresent.push_back(platformObject2D);

	// Player Object 2D.
	setTransform2d = Transform2D();
	setTransform2d.SetPosition(0.0f, 5.0f);
	PlayerObject2D* playerObject2D = new PlayerObject2D(setTransform2d);
	this->objects2DPresent.push_back(playerObject2D);

	// Camera Object 2D.
	setTransform2d = Transform2D();
	CameraObject2D* cameraObject = new CameraObject2D(setTransform2d);
	cameraObject->FollowPosition(playerObject2D->GetTransform2D());
	this->objects2DPresent.push_back(cameraObject);
}