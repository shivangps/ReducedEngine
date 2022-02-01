#pragma once
#include <box2d/box2d.h>
#include <box2d/b2_settings.h>

#include "../../MathEngine.h"
#include "Colliders/Collider2D.h"

// A component that handles the physical characteristics of the object.
class GameObject2D;
enum RigibodyType
{
	RigibodyTypeStatic = 0,
	RigibodyTypeDynamic,
	RigibodyTypeKinematic
};

class Rigidbody2D
{
private:
	Transform2D* transform = nullptr;

	bool isStatic = false;
	b2BodyDef bodyDefination;
	b2MassData massData;
	b2Body* physicsBody = nullptr;
	float gravityScale = 1.0f;
	PhysicalCharacteristics2D* gameObjectPhysicalCharacteristics = nullptr;
	std::vector<Collider2D*> collidersAssigned = {};

public:
	Rigidbody2D(Transform2D* transform, bool staticState, RigibodyType type);

	// Function to add the collider to the body.
	void AddColliderToRigidbody(Collider2D* collider);
	// Function to initalize the colliders and rigidbody to physics world.
	void Initialize(b2World* physicsWorld);
	// Function to update the transform of the game object.
	void UpdateTransform();
	// Function to translate the rigidbody.
	void TranslateBody(Vector2 translationVector);
	// Function to set mass of the rigidbody.
	void SetMass(float mass);
	// Function to add the user data.
	void SetPhysicalCharacteristics(PhysicalCharacteristics2D* characteristics);
};