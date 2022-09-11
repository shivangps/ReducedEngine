#include "GameObject2D.h"
#include "Scene.h"

void GameObject2D::SetAsStatic()
{
	this->isStatic = true;
}

void GameObject2D::SetAsDynamic()
{
	this->isStatic = false;
}

bool GameObject2D::GetStaticState()
{
	return this->isStatic;
}

bool GameObject2D::CompareTag(std::string otherTag)
{
	return std::strcmp(this->tag.c_str(), otherTag.c_str()) == 0;
}

void GameObject2D::SetScene(Scene* scene)
{
	this->associatedScene = scene;
}

bool GameObject2D::GetEnabledState()
{
	return this->isEnabled;
}

void GameObject2D::EnableGameObject()
{
	this->isEnabled = true;
}

void GameObject2D::DisableGameObject()
{
	this->isEnabled = false;
}
