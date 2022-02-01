#pragma once
#include <box2d/b2_polygon_shape.h>
#include "Collider2D.h"

// Class containing the body collider defination for a box shape.

class BoxCollider2D : public Collider2D
{
public:
	BoxCollider2D(Transform2D* transform);

	void CalculateColliderDimensions();
};