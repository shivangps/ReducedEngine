#include "BoxObject.h"

#include "../Assets/TextureShader/TextureShader.h"

BoxObject::BoxObject()
{
	this->renderComponent.LoadModel("Assets/3D_Objects/Crate/Crate.obj");
	this->renderComponent.SetShaderToComponent(TextureShader::GetInstance());
}

BoxObject::BoxObject(Transform transform)
{
	this->objectTransform = transform;
	this->renderComponent.LoadModel("Assets/3D_Objects/Crate/Crate.obj");
	this->renderComponent.SetShaderToComponent(TextureShader::GetInstance());
}

void BoxObject::Initialize(RenderList* renderList)
{
	renderList->AssignRenderComponent(&this->renderComponent);
}

void BoxObject::Update()
{
	this->objectTransform.RotateY(0.01f * Time::GetInstance()->GetDeltaTime());
}
