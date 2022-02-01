#pragma once
#include "../../../MathEngine.h"
#include <box2d/b2_polygon_shape.h>

// This contains the class defination of the base collider.

class Collider2D
{
protected:
	Transform2D* transform = nullptr;

	Vector2 offset = Vector2(0.0f, 0.0f);

	b2PolygonShape colliderShape;
	bool isTrigger = false;
	float density = 0.5f;
	float friction = 0.5f;

public:
	// Constructor to initialize using the transform 2D pointer.
	Collider2D(Transform2D* transform) 
	{
		this->transform = transform;
	}
	// Function to offset the collider shape from the main position based on the transform.
	void SetOffset(Vector2 offset)
	{
		this->offset = offset;
	}
	// Function to get the offset position.
	Vector2 GetOffset();
	// Function to get the pointer to the collider shape.
	b2PolygonShape* GetShape();
	// Function to set the physical material characteristics.
	void SetDensity(float value);
	float GetDensity();
	void SetFriction(float value);
	float GetFriction();
	void SetAsTrigger(bool triggerBool);
	bool GetTriggerState();
	//Function to calculate the collider mesh.
	virtual void CalculateColliderDimensions() {}
};