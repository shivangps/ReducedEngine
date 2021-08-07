#include "BoxObject.h"

#include "../Assets/TextureShader/TextureShader.h"

void BoxObject::LoadCubeMesh()
{
	std::vector<MeshVertex> vertices;

	vertices.push_back(MeshVertex(0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f));
	vertices.push_back(MeshVertex(-0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f));
	vertices.push_back(MeshVertex(-0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f));
	vertices.push_back(MeshVertex(0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f));

	vertices.push_back(MeshVertex(0.5f, -0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f));
	vertices.push_back(MeshVertex(0.5f, 0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f));
	vertices.push_back(MeshVertex(-0.5f, 0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f));
	vertices.push_back(MeshVertex(-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f));

	vertices.push_back(MeshVertex(0.5f, -0.5f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f));
	vertices.push_back(MeshVertex(0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f));
	vertices.push_back(MeshVertex(-0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f));
	vertices.push_back(MeshVertex(-0.5f, -0.5f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f));

	vertices.push_back(MeshVertex(0.5f, 0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f));
	vertices.push_back(MeshVertex(0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f));
	vertices.push_back(MeshVertex(-0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f));
	vertices.push_back(MeshVertex(-0.5f, 0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f));

	vertices.push_back(MeshVertex(0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f));
	vertices.push_back(MeshVertex(0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f));
	vertices.push_back(MeshVertex(0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f));
	vertices.push_back(MeshVertex(0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f));

	vertices.push_back(MeshVertex(-0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f));
	vertices.push_back(MeshVertex(-0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f));
	vertices.push_back(MeshVertex(-0.5f, 0.5f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f));
	vertices.push_back(MeshVertex(-0.5f, -0.5f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f));

	std::vector<unsigned short> indices;

	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);

	indices.push_back(0);
	indices.push_back(2);
	indices.push_back(3);

	indices.push_back(7);
	indices.push_back(6);
	indices.push_back(5);

	indices.push_back(7);
	indices.push_back(5);
	indices.push_back(4);

	indices.push_back(8);
	indices.push_back(9);
	indices.push_back(10);

	indices.push_back(8);
	indices.push_back(10);
	indices.push_back(11);

	indices.push_back(13);
	indices.push_back(12);
	indices.push_back(14);

	indices.push_back(14);
	indices.push_back(12);
	indices.push_back(15);

	indices.push_back(16);
	indices.push_back(17);
	indices.push_back(19);

	indices.push_back(19);
	indices.push_back(17);
	indices.push_back(18);

	indices.push_back(20);
	indices.push_back(21);
	indices.push_back(23);

	indices.push_back(23);
	indices.push_back(21);
	indices.push_back(22);
}

BoxObject::BoxObject()
{
	this->objectTransform.SetPosition(0.0f, 2.0f, 0.0f);
	this->objectTransform.SetRotation(0.0f, 45.0f, 0.0f);
	this->objectTransform.SetScale(Vector3(0.5f));

	this->renderComponent.LoadModel("Assets/3D_Objects/Crate/Crate.obj");
	this->renderComponent.SetShaderToComponent(TextureShader::GetInstance());
}

void BoxObject::Initialize(RenderList* renderList)
{
	renderList->AssignRenderComponent(&this->renderComponent);
}

void BoxObject::Update()
{
}
