#pragma once
#include "../GameObject.h"

class BoxObject : public GameObject
{
private:
	RenderComponent renderComponent = { &this->objectTransform };

	float rotationX = 0.0f;
public:
	BoxObject();
	BoxObject(Transform transform);

	void Initialize(RenderList* renderList);

	void Update();
};