#include "PlaneObject.h"

#include "../Assets/TextureShader/TextureShader.h"

PlaneObject::PlaneObject()
{
	//this->LoadPlaneMesh();
	this->renderComponent.LoadModel("Assets/3D_Objects/Plane/Plane.obj");
	this->renderComponent.SetShaderToComponent(TextureShader::GetInstance());
}

PlaneObject::PlaneObject(Transform transform)
{
	this->objectTransform = transform;
	//this->LoadPlaneMesh();
	this->renderComponent.LoadModel("Assets/3D_Objects/Plane/Plane.obj");
	this->renderComponent.SetShaderToComponent(TextureShader::GetInstance());
}

void PlaneObject::Initialize(RenderList* renderList)
{
	renderList->AssignRenderComponent(&this->renderComponent);
}

void PlaneObject::Update()
{
}
