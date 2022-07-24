#include "GameObject2D.h"

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
