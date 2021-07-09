
#include "ErrorLogger.h"
#include "Scene.h"

unsigned int Scene::GetSize()
{
	return objectsPresent.size();
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

void Scene::SetGameObject(GameObject* newGameObject)
{
	this->objectsPresent.push_back(newGameObject);
}

RenderList* Scene::GetRenderComponentList()
{
	return &this->renderComponentList;
}
