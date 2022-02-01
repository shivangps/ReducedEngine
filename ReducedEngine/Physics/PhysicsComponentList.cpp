#include "PhysicsComponentList.h"

PhysicsComponentList::PhysicsComponentList()
{
	this->physicsWorld = new b2World(this->gravity.GetBox2DVector());
	this->physicsWorld->SetContactListener(this);
	this->physicsWorld->SetGravity(this->gravity.GetBox2DVector());
}

void PhysicsComponentList::RegisterRigidbody2D(Rigidbody2D* rigidbody2D)
{
	rigidbody2D->Initialize(this->physicsWorld);
	this->rigidbodiesPresent.push_back(rigidbody2D);
}

void PhysicsComponentList::Step(float deltaTime)
{
	this->physicsWorld->Step(deltaTime, 6, 1);

	for (unsigned int i = 0; i < this->rigidbodiesPresent.size(); i++)
	{
		this->rigidbodiesPresent[i]->UpdateTransform();
	}
}

void PhysicsComponentList::BeginContact(b2Contact* contact)
{
	b2BodyUserData firstUserData = contact->GetFixtureA()->GetBody()->GetUserData();
	firstUserData.characteristics->OnCollisionEnter2D();

	b2BodyUserData secondUserData = contact->GetFixtureB()->GetBody()->GetUserData();
	secondUserData.characteristics->OnCollisionEnter2D();
}

void PhysicsComponentList::EndContact(b2Contact* contact)
{
	b2BodyUserData firstUserData = contact->GetFixtureA()->GetBody()->GetUserData();
	firstUserData.characteristics->OnCollisionExit2D();

	b2BodyUserData secondUserData = contact->GetFixtureB()->GetBody()->GetUserData();
	secondUserData.characteristics->OnCollisionExit2D();
}
