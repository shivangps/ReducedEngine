#pragma once
#include <box2d/box2d.h>
#include "../MathEngine.h"
#include "2D/Rigidbody2D.h"

// This class contains all the data related ot physics of both 2D or 3D in the scene.

class PhysicsComponentList : public b2ContactListener
{
private:
	// BOX2D.
	// Construct a world object, which will hold and simulate the rigid bodies.
	b2World* physicsWorld = nullptr;

	// Define the gravity vector.
	Vector2 gravity = Vector2(0.0f, -9.8f);

	// Vector to maintain the rigidbody to initialize to the ohysics engine.
	std::vector<Rigidbody2D*> rigidbodiesPresent = {};

public:
	PhysicsComponentList();

	// Function to register the rigidbody to physics list.
	void RegisterRigidbody2D(Rigidbody2D* rigidbody2D);
	// Function to step the physics world and constituent physical objects to process collision and movement.
	void Step(float deltaTime);

	/// Called when two fixtures begin to touch.
	void BeginContact(b2Contact* contact);

	/// Called when two fixtures cease to touch.
	void EndContact(b2Contact* contact);
};