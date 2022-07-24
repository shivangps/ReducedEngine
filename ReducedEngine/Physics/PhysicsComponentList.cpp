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
	b2BodyUserData secondUserData = contact->GetFixtureB()->GetBody()->GetUserData();

	CollisionCharacteristics2D firstUserCollision = CollisionCharacteristics2D();
	CollisionCharacteristics2D secondUserCollision = CollisionCharacteristics2D();
	//b2WorldManifold worldManifold;
	//contact->GetWorldManifold(&worldManifold);

	firstUserCollision.otherCollidingObject = secondUserData.characteristics->gameObject;
	firstUserCollision.collisionPosition = new Vector2();
	firstUserCollision.collisionNormal = new Vector2();
	firstUserData.characteristics->OnCollisionEnter2D();
	firstUserData.characteristics->OnCollisionEnter2D(firstUserCollision);
	
	secondUserCollision.otherCollidingObject = firstUserData.characteristics->gameObject; 
	secondUserCollision.collisionPosition = new Vector2();
	secondUserCollision.collisionNormal = new Vector2();
	secondUserData.characteristics->OnCollisionEnter2D();
	secondUserData.characteristics->OnCollisionEnter2D(secondUserCollision);
}

void PhysicsComponentList::EndContact(b2Contact* contact)
{
	b2BodyUserData firstUserData = contact->GetFixtureA()->GetBody()->GetUserData();
	firstUserData.characteristics->OnCollisionExit2D();

	b2BodyUserData secondUserData = contact->GetFixtureB()->GetBody()->GetUserData();
	secondUserData.characteristics->OnCollisionExit2D();
}
