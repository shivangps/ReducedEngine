#include "PongMenuScene.h"
#include "../Assets/2D_Assets/AtariPong/PongMenuObjects/PongMenuManager.h"
#include "../Assets/2D_Assets/Objects/CameraObject2D/CameraObject2D.h"
#include "../Assets/2D_Assets/AtariPong/PongMenuObjects/PlayButtonObject.h"


PongMenuScene::PongMenuScene()
{
	GameObject2D* newGameobject2D = nullptr;
	Transform2D setTransform2d;

	// Camera Object 2D.
	setTransform2d = Transform2D();
	CameraObject2D* cameraObject = new CameraObject2D(setTransform2d);
	this->objects2DPresent.push_back(cameraObject);

	// Play Button Object.
	setTransform2d = Transform2D();
	PlayButtonObject* playButton = new PlayButtonObject(setTransform2d);
	this->objects2DPresent.push_back(playButton);

	// Menu Object 2D.
	setTransform2d = Transform2D();
	PongMenuManager* menuManager = new PongMenuManager(setTransform2d);
	this->objects2DPresent.push_back(menuManager);
	menuManager->AssignPlayButtonObject(playButton);
}
