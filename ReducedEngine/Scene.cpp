
#include "ErrorLogger.h"
#include "Scene.h"

unsigned int Scene::GetSize()
{
	return objectsPresent.size();
}

unsigned int Scene::GetSize2D()
{
	return this->objects2DPresent.size();
}

bool Scene::CheckIndexSanity(unsigned int index)
{
	unsigned int collectionSize = objectsPresent.size();

	if (index < collectionSize)
		return true;

	return false;
}

GameObject* Scene::GetGameObject(unsigned int index)
{
	if (CheckIndexSanity(index))
	{
		return objectsPresent[index];
	}
	else
	{
		ErrorLog("Index exceeding the number of objects present in scene.");
	}
		
	exit(-1);
}

GameObject2D* Scene::GetGameObject2D(unsigned int index)
{
	if (CheckIndexSanity2D(index))
	{
		return objects2DPresent[index];
	}
	else
	{
		ErrorLog("Index exceeding the number of 2D objects present in scene.");
	}

	exit(-1);
}

bool Scene::CheckIndexSanity2D(unsigned int index)
{
	unsigned int collectionSize = objects2DPresent.size();

	if (index < collectionSize)
		return true;

	return false;
}

void Scene::SetGameObject(GameObject* newGameObject)
{
	this->objectsPresent.push_back(newGameObject);
}

void Scene::SetGameObject2D(GameObject2D* newGameObject2d)
{
	this->objects2DPresent.push_back(newGameObject2d);
}

RenderList* Scene::GetRenderComponentList()
{
	return &this->renderComponentList;
}

PhysicsComponentList* Scene::GetPhysicsComponentList()
{
	return &this->physicsComponentList;
}

