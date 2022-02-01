#include "Collider2D.h"

Vector2 Collider2D::GetOffset()
{
	return this->offset;
}

b2PolygonShape* Collider2D::GetShape()
{
	return &this->colliderShape;
}

void Collider2D::SetDensity(float value)
{
	this->density = value;
}

float Collider2D::GetDensity()
{
	return this->density;
}

void Collider2D::SetFriction(float value)
{
	this->friction = value;
}

float Collider2D::GetFriction()
{
	return this->friction;
}

void Collider2D::SetAsTrigger(bool triggerBool)
{
	this->isTrigger = triggerBool;
}

bool Collider2D::GetTriggerState()
{
	return this->isTrigger;
}
