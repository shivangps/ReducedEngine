#include "MainScene.h"
#include "../Assets/Objects/Box/BoxObject.h"
#include "../Assets/Objects/Player/PlayerObject.h"

MainScene::MainScene()
{
	GameObject* newObject = nullptr;

	newObject = new BoxObject();
	newObject->Initialize(&this->renderComponentList);
	this->objectsPresent.push_back(newObject);

	newObject = new PlayerObject();
	newObject->Initialize(&this->renderComponentList);
	this->objectsPresent.push_back(newObject);
}
