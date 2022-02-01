#include "BoxCollider2D.h"

BoxCollider2D::BoxCollider2D(Transform2D* transform) : Collider2D(transform)
{
	this->CalculateColliderDimensions();
}

void BoxCollider2D::CalculateColliderDimensions()
{
	Vector2 scale = this->transform->GetGlobalScale();
	Vector2 position = this->transform->GetGlobalPostion();
	float angle = this->transform->GetGlobalRotation();
	// From the center point hence half the width and height.
	this->colliderShape.SetAsBox(scale.X() / 2.0f, scale.Y() / 2.0f, offset.GetBox2DVector(), angle);
}