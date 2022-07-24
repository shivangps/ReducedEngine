#include "BoxObject2D.h"
#include "../Assets/2D_Assets/RenderComponents/WireframeRenderComponent2D.h"

BoxObject2D::BoxObject2D(Transform2D transform)
{
	this->objectTransform = transform;
}

void BoxObject2D::Initialize(RenderList* sceneRenderComponentList, PhysicsComponentList* physicsComponentList, AudioComponentList* audioComponentList, GUIComponentList* guiComponentList)
{
	this->isStatic = true;

	WireframeRenderComponent2D* newRenderComponent2D = new WireframeRenderComponent2D(&this->objectTransform);

	std::vector<MeshVertex2D> vertices;
	vertices.push_back(MeshVertex2D(-0.5f, 0.5f, 0.0f, 1.0f));
	vertices.push_back(MeshVertex2D(-0.5f,-0.5f, 0.0f, 0.0f));
	vertices.push_back(MeshVertex2D( 0.5f, 0.5f, 1.0f, 1.0f));
	vertices.push_back(MeshVertex2D( 0.5f,-0.5f, 1.0f, 0.0f));

	std::vector<unsigned short> indices;
	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);

	indices.push_back(1);
	indices.push_back(2);
	indices.push_back(3);

	newRenderComponent2D->Load2DGeometry(vertices, indices);

	// Change the object color.
	newRenderComponent2D->ChangeColor(this->color);

	this->wireframeRenderer = newRenderComponent2D;

	sceneRenderComponentList->AssignRenderComponent2D(this->wireframeRenderer);

	// Physics.
	this->boxCollider = new BoxCollider2D(&this->objectTransform);

	this->rigidbody = new Rigidbody2D(&this->objectTransform, this, this->isStatic, RigibodyType::RigibodyTypeStatic, true);
	this->rigidbody->AddColliderToRigidbody(this->boxCollider);
	this->rigidbody->SetPhysicalCharacteristics(this);
	physicsComponentList->RegisterRigidbody2D(this->rigidbody);
}

void BoxObject2D::Update()
{

}
