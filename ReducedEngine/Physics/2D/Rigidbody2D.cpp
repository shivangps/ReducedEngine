#include "Rigidbody2D.h"

Rigidbody2D::Rigidbody2D(Transform2D* transform, GameObject2D* gameObject, bool staticState, RigibodyType type, bool fixedRotation = false)
{
	this->transform = transform;

	this->type = type;

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

	this->bodyDefination.fixedRotation = fixedRotation;

	PhysicalCharacteristics2D* characteristics = (PhysicalCharacteristics2D*)gameObject;
	characteristics->gameObject = gameObject;

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

void Rigidbody2D::ChangeTransform(Transform2D transform)
{
	if (this->type == RigibodyType::RigibodyTypeKinematic)
	{
		this->physicsBody->SetTransform(transform.GetGlobalPostion().GetBox2DVector(), transform.GetGlobalRotation());
		this->physicsBody->SetAwake(true);
	}
}

void Rigidbody2D::SetVelocity(Vector2 vector)
{
	this->physicsBody->SetLinearVelocity(vector.GetBox2DVector());
}

Vector2 Rigidbody2D::GetVelocity()
{
	return this->physicsBody->GetLinearVelocity();
}

void Rigidbody2D::SetToPosition(Vector2 position)
{
	this->physicsBody->SetTransform(position.GetBox2DVector(), this->transform->GetGlobalRotation());
}

void Rigidbody2D::SetMass(float mass)
{
	this->massData.mass = mass;
}

void Rigidbody2D::SetGravityScale(float scale)
{
	this->gravityScale = scale;
}

void Rigidbody2D::SetPhysicalCharacteristics(PhysicalCharacteristics2D* characteristics)
{
	this->gameObjectPhysicalCharacteristics = characteristics;
}
