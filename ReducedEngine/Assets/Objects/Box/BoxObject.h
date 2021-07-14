#pragma once
#include "../GameObject.h"

class BoxObject : public GameObject
{
private:
	RenderComponent renderComponent = { &this->objectTransform };

	void LoadCubeMesh();

	float rotationX = 0.0f;
public:
	BoxObject();

	void Initialize(RenderList* renderList);

	void Update();
};