#pragma once
#include "../GameObject.h"

class BoxObject : public GameObject
{
private:
	RenderComponent renderComponent = { &this->objectTransform };

	void LoadCubeMesh();
public:
	BoxObject();

	void Initialize(RenderList* renderList);
};