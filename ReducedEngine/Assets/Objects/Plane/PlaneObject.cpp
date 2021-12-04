#include "PlaneObject.h"

#include "../Assets/TextureShader/TextureShader.h"

void PlaneObject::LoadPlaneMesh()
{
	std::vector<MeshVertex> vertices;

	vertices.push_back(MeshVertex( 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f));
	vertices.push_back(MeshVertex( 1.0f, 0.0f,-1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f));
	vertices.push_back(MeshVertex(-1.0f, 0.0f,-1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f));
	vertices.push_back(MeshVertex(-1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f));

	std::vector<unsigned short> indices;

	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);

	indices.push_back(0);
	indices.push_back(2);
	indices.push_back(3);

	this->renderComponent.InitializeNewMesh(vertices, indices);
}

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
