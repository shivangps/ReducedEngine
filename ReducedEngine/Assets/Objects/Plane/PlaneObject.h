#pragma once
#include "../GameObject.h"

class PlaneObject : public GameObject
{
private:
	RenderComponent renderComponent = { &this->objectTransform };

public:
	PlaneObject();
	PlaneObject(Transform transform);

	void Initialize(RenderList* renderList);

	void Update();
};