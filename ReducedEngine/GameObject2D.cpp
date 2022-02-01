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
