#include "MainScene.h"
#include "../Assets/Objects/Box/BoxObject.h"
#include "../Assets/Objects/Plane/PlaneObject.h"
#include "../Assets/Objects/Player/PlayerObject.h"

MainScene::MainScene()
{
	GameObject* newObject = nullptr;
	Transform setTransform;
	setTransform = Transform();

	newObject = new BoxObject(setTransform);
	newObject->Initialize(&this->renderComponentList);
	this->objectsPresent.push_back(newObject);
	setTransform = Transform();

	setTransform.SetPosition(0.0f,-1.1f, 0.0f);
	setTransform.SetScale(3.0f);
	newObject = new PlaneObject(setTransform);
	newObject->Initialize(&this->renderComponentList);
	this->objectsPresent.push_back(newObject);
	setTransform = Transform();

	newObject = new PlayerObject();
	newObject->Initialize(&this->renderComponentList);
	this->objectsPresent.push_back(newObject);
}
