#include "Rigidbody2D.h"

Rigidbody2D::Rigidbody2D(Transform2D* transform, bool staticState, RigibodyType type)
{
	this->transform = transform;

	Vector2 newPos = transform->GetGlobalPostion();
	this->bodyDefination.position.Set(newPos.X(), newPos.Y());

	this->isStatic = staticState;

	if (staticState)
	{
		this->bodyDefination.type = b2_staticBody;
	}
	else
	{
		if (type == RigibodyType::RigibodyTypeKinematic)
		{
			this->bodyDefination.type = b2_kinematicBody;
		}
		else if (type == RigibodyType::RigibodyTypeDynamic)
		{
			this->bodyDefination.type = b2_dynamicBody;
		}
		else
		{
			this->bodyDefination.type = b2_staticBody;
		}
	}

	this->massData.mass = 1.0f;
}

void Rigidbody2D::AddColliderToRigidbody(Collider2D* collider)
{
	this->collidersAssigned.push_back(collider);
}

void Rigidbody2D::Initialize(b2World* physicsWorld)
{
	// Set the body to the physics world.
	this->physicsBody = physicsWorld->CreateBody(&this->bodyDefination);

	b2BodyUserData userData;
	userData.characteristics = this->gameObjectPhysicalCharacteristics;
	this->physicsBody->SetUserData(userData);

	this->physicsBody->SetMassData(&this->massData);

	this->physicsBody->SetGravityScale(this->gravityScale);

	b2FixtureDef fixtureDef;

	// Set all the colliders to the rigidbody by applying fixtures.
	for (unsigned int i = 0; i < this->collidersAssigned.size(); i++)
	{
		Collider2D* collider = this->collidersAssigned[i];

		fixtureDef.shape = collider->GetShape();
		fixtureDef.density = collider->GetDensity();
		fixtureDef.friction = collider->GetFriction();
		fixtureDef.isSensor = collider->GetTriggerState();

		this->physicsBody->CreateFixture(&fixtureDef);
	}
}

void Rigidbody2D::UpdateTransform()
{
	if (!this->isStatic)
	{
		this->transform->SetPosition(this->physicsBody->GetPosition());
		this->transform->SetRotation(this->physicsBody->GetAngle());
	}
}

void Rigidbody2D::TranslateBody(Vector2 translationVector)
{
	this->physicsBody->ApplyLinearImpulseToCenter(translationVector.GetBox2DVector(), true);
}

void Rigidbody2D::SetMass(float mass)
{
	this->massData.mass = mass;
}

void Rigidbody2D::SetPhysicalCharacteristics(PhysicalCharacteristics2D* characteristics)
{
	this->gameObjectPhysicalCharacteristics = characteristics;
}
