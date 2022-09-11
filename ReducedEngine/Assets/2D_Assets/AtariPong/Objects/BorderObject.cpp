#include "BorderObject.h"

void BorderObject::Initialize(RenderList* sceneRenderComponentList, PhysicsComponentList* physicsComponentList, AudioComponentList* audioComponentList, GUIComponentList* guiComponentList)
{
	this->tag = "Border";

	this->objectTransform.SetScale(this->size);

	// Render component initialization.
	if (this->toRender)
	{
		std::vector<MeshVertex2D> vertices;
		vertices.push_back(MeshVertex2D(-0.5f, 0.5f, 0.0f, 1.0f));
		vertices.push_back(MeshVertex2D(-0.5f, -0.5f, 0.0f, 0.0f));
		vertices.push_back(MeshVertex2D(0.5f, 0.5f, 1.0f, 1.0f));
		vertices.push_back(MeshVertex2D(0.5f, -0.5f, 1.0f, 0.0f));

		std::vector<unsigned short> indices;
		indices.push_back(0);
		indices.push_back(1);
		indices.push_back(2);

		indices.push_back(1);
		indices.push_back(2);
		indices.push_back(3);

		this->renderComponent.Load2DGeometry(vertices, indices);

		this->renderComponent.ChangeColor(Vector3(1.0f));

		this->renderComponent.SetGameObject(this);

		sceneRenderComponentList->AssignRenderComponent2D(&this->renderComponent);
	}

	// Physics components initilazation.
	this->boxCollider = BoxCollider2D(&this->objectTransform);
	this->boxCollider.SetAsTrigger(true);
	this->rigidbody = new Rigidbody2D(&this->objectTransform, this, false, RigibodyType::RigibodyTypeDynamic, true);
	this->rigidbody->AddColliderToRigidbody(&this->boxCollider);
	this->rigidbody->SetPhysicalCharacteristics(this);
	this->rigidbody->SetGravityScale(0.0f);
	this->rigidbody->SetMass(0.1f);

	physicsComponentList->RegisterRigidbody2D(this->rigidbody);
}

void BorderObject::Update()
{
}
