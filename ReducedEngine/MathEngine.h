#pragma once

#include "CommonHeader.h"
#include "box2d/b2_math.h"

// Constants that store the normalized values of 3 axes.
const DirectX::XMVECTOR global_right = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
const DirectX::XMVECTOR global_up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
const DirectX::XMVECTOR global_forward = DirectX::XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);

// Structure to handle 2 float values namely X and Y. Representing vector in 2D.
struct Vector2
{
private:
	DirectX::XMFLOAT2 values = DirectX::XMFLOAT2(0.0f, 0.0f);

	Vector2 Multiply(Vector2 vector, float aValue)
	{
		return Vector2(aValue * vector.X(), aValue * vector.Y());
	}
	Vector2 Multiply(Vector2 firstVector, Vector2 secondVector)
	{
		return Vector2(firstVector.X() * secondVector.X(), firstVector.Y() * secondVector.Y());
	}
	Vector2 Divide(Vector2 vector, float aValue)
	{
		return Vector2(vector.X() / aValue, vector.Y() / aValue);
	}

public:
	Vector2() {}
	Vector2(float x, float y) : values(x, y) {}
	Vector2(DirectX::XMFLOAT2 newValues) : values(newValues) {}
	Vector2(DirectX::XMVECTOR vector)
	{
		XMStoreFloat2(&this->values, vector);
	}
	Vector2(float aValue) : values(aValue, aValue) {}
	Vector2(b2Vec2 box2dVector2) : values(box2dVector2.x, box2dVector2.y) {}

	// Get & Set functions.
	DirectX::XMVECTOR GetVector()
	{
		return DirectX::XMLoadFloat2(&this->values);
	}
	DirectX::XMFLOAT2 GetFloat2()
	{
		return this->values;
	}
	float X() { return this->values.x; }
	float Y() { return this->values.y; }
	void X(float x) { this->values.x = x; }
	void Y(float y) { this->values.y = y; }

	// Operations. (Using SIMD operations)
	void operator=(DirectX::XMFLOAT2 newValues)
	{
		this->values = newValues;
	}
	void operator=(DirectX::XMVECTOR vector)
	{
		DirectX::XMStoreFloat2(&this->values, vector);
	}
	Vector2 operator+(Vector2 otherVector2)
	{
		return DirectX::XMVectorAdd(this->GetVector(), otherVector2.GetVector());
	}
	Vector2 operator+(DirectX::XMFLOAT2 otherFloat2)
	{
		return DirectX::XMVectorAdd(this->GetVector(), XMLoadFloat2(&otherFloat2));
	}
	Vector2 operator+(DirectX::XMVECTOR otherVector)
	{
		return DirectX::XMVectorAdd(this->GetVector(), otherVector);
	}
	Vector2 operator+(float aValue)
	{
		return Vector2(this->X() + aValue, this->Y() + aValue);
	}
	void operator+=(Vector2 otherVector2)
	{
		*this = *this + otherVector2;
	}
	Vector2 operator-(Vector2 otherVector2)
	{
		return DirectX::XMVectorSubtract(this->GetVector(), otherVector2.GetVector());
	}
	Vector2 operator-()
	{
		return Vector2(-this->X(), -this->Y());
	}
	Vector2 operator*(float aValue)
	{
		return this->Multiply(*this, aValue);
	}
	Vector2 operator*(Vector2 otherVector2)
	{
		return this->Multiply(*this, otherVector2);
	}
	void operator*=(Vector2 otherVector2)
	{
		*this = this->Multiply(*this, otherVector2);
	}
	void operator*=(float aValue)
	{
		*this = this->Multiply(*this, aValue);
	}
	Vector2 operator/(float aValue)
	{
		return this->Divide(*this, aValue);
	}
	Vector2 Normalize()
	{
		return DirectX::XMVector2Normalize(this->GetVector());
	}
	Vector2 Reflect(Vector2 normal)
	{
		return DirectX::XMVector2Reflect(this->GetVector(), normal.GetVector());
	}
	b2Vec2 GetBox2DVector()
	{
		return b2Vec2(this->values.x, this->values.y);
	}
};

// Structure to handle 3 float values namely X, Y and Z. Representing a vector.
struct Vector3
{
private:
	DirectX::XMFLOAT3 values = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);

	Vector3 Multiply(Vector3 vector, float aValue)
	{
		return Vector3(aValue * vector.X(), aValue * vector.Y(), aValue * vector.Z());
	}
	Vector3 Multiply(Vector3 firstVector, Vector3 secondVector)
	{
		return Vector3(firstVector.X() * secondVector.X(), firstVector.Y() * secondVector.Y(), firstVector.Z() * secondVector.Z());
	}
	Vector3 Divide(Vector3 vector, float aValue)
	{
		return Vector3(vector.X() / aValue, vector.Y() / aValue, vector.Z() / aValue);
	}
public:
	Vector3() {}
	Vector3(float x, float y, float z) : values(x, y, z) {}
	Vector3(DirectX::XMFLOAT3 newValues) : values(newValues) {}
	Vector3(DirectX::XMVECTOR vector)
	{
		XMStoreFloat3(&this->values, vector);
	}
	Vector3(float aValue) : values(aValue, aValue, aValue) {}
	Vector3(Vector2 vector2d) : values(vector2d.X(), vector2d.Y(), 0.0f) {}
	Vector3(b2Vec3 box2dVector3) : values(box2dVector3.x, box2dVector3.y, box2dVector3.z) {}

	// Get & Set functions.
	DirectX::XMVECTOR GetVector()
	{
		return DirectX::XMLoadFloat3(&this->values);
	}
	DirectX::XMFLOAT3 GetFloat3()
	{
		return this->values;
	}
	float X() { return this->values.x; }
	float Y() { return this->values.y; }
	float Z() { return this->values.z; }
	void X(float x) { this->values.x = x; }
	void Y(float y) { this->values.y = y; }
	void Z(float z) { this->values.z = z; }

	// Operations. (Using SIMD operations)
	void operator=(DirectX::XMFLOAT3 newValues)
	{
		this->values = newValues;
	}
	void operator=(DirectX::XMVECTOR vector)
	{
		DirectX::XMStoreFloat3(&this->values, vector);
	}
	Vector3 operator+(Vector3 otherVector3)
	{
		return DirectX::XMVectorAdd(this->GetVector(), otherVector3.GetVector());
	}
	Vector3 operator+(DirectX::XMFLOAT3 otherFloat3)
	{
		return DirectX::XMVectorAdd(this->GetVector(), XMLoadFloat3(&otherFloat3));
	}
	Vector3 operator+(DirectX::XMVECTOR otherVector)
	{
		return DirectX::XMVectorAdd(this->GetVector(), otherVector);
	}
	Vector3 operator+(float aValue)
	{
		return Vector3(this->X() + aValue, this->Y() + aValue, this->Z() + aValue);
	}
	void operator+=(Vector3 otherVector3)
	{
		*this = *this + otherVector3;
	}
	Vector3 operator-(Vector3 otherVector3)
	{
		return DirectX::XMVectorSubtract(this->GetVector(), otherVector3.GetVector());
	}
	Vector3 operator-()
	{
		return Vector3(-this->X(), -this->Y(), -this->Z());
	}
	Vector3 operator*(float aValue)
	{
		return this->Multiply(*this, aValue);
	}
	void operator*=(Vector3 otherVector3)
	{
		*this = this->Multiply(*this, otherVector3);
	}
	void operator*=(float aValue)
	{
		*this = this->Multiply(*this, aValue);
	}
	Vector3 operator/(float aValue)
	{
		return this->Divide(*this, aValue);
	}
	Vector3 Normalize()
	{
		return DirectX::XMVector3Normalize(this->GetVector());
	}
	b2Vec3 GetBox2DVector()
	{
		return b2Vec3(this->values.x, this->values.y, this->values.z);
	}
};

// Structure to handle and operate on 4 x 4 matrix float.
struct Matrix4
{
private:
	DirectX::XMFLOAT4X4 values = {};

public:
	Matrix4()
	{
		values = DirectX::XMFLOAT4X4(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	}
	Matrix4(DirectX::XMMATRIX matrix)
	{
		DirectX::XMStoreFloat4x4(&this->values, matrix);
	}
	Matrix4(DirectX::XMFLOAT4X4 float4x4)
	{
		this->values = float4x4;
	}
	Matrix4(float m00, float m01, float m02, float m03, float m10, float m11, float m12, float m13, float m20, float m21, float m22, float m23, float m30, float m31, float m32, float m33)
	{
		this->values._11 = m00; this->values._12 = m01; this->values._13 = m02; this->values._14 = m03;
		this->values._21 = m10; this->values._22 = m11; this->values._23 = m12; this->values._24 = m13;
		this->values._31 = m20; this->values._32 = m21; this->values._33 = m22; this->values._34 = m23;
		this->values._41 = m30; this->values._42 = m31; this->values._43 = m32; this->values._44 = m33;
	}

	// Get & Set functions.
	DirectX::XMMATRIX GetMatrix()
	{
		return XMLoadFloat4x4(&this->values);
	}
	DirectX::XMFLOAT4X4 GetFloat4x4()
	{
		return this->values;
	}
	float Get11() { return this->values._11; }
	float Get12() { return this->values._12; }
	float Get13() { return this->values._13; }
	float Get14() { return this->values._14; }
	float Get21() { return this->values._21; }
	float Get22() { return this->values._22; }
	float Get23() { return this->values._23; }
	float Get24() { return this->values._24; }
	float Get31() { return this->values._31; }
	float Get32() { return this->values._32; }
	float Get33() { return this->values._33; }
	float Get34() { return this->values._34; }
	float Get41() { return this->values._41; }
	float Get42() { return this->values._42; }
	float Get43() { return this->values._43; }
	float Get44() { return this->values._44; }
	void Set11(float value) { this->values._11 = value; }
	void Set12(float value) { this->values._12 = value; }
	void Set13(float value) { this->values._13 = value; }
	void Set14(float value) { this->values._14 = value; }
	void Set21(float value) { this->values._21 = value; }
	void Set22(float value) { this->values._22 = value; }
	void Set23(float value) { this->values._23 = value; }
	void Set24(float value) { this->values._24 = value; }
	void Set31(float value) { this->values._31 = value; }
	void Set32(float value) { this->values._32 = value; }
	void Set33(float value) { this->values._33 = value; }
	void Set34(float value) { this->values._34 = value; }
	void Set41(float value) { this->values._41 = value; }
	void Set42(float value) { this->values._42 = value; }
	void Set43(float value) { this->values._43 = value; }
	void Set44(float value) { this->values._44 = value; }

	// Operations. (Using SIMD operations)
	void operator=(DirectX::XMFLOAT4X4 newValues)
	{
		this->values = newValues;
	}
	void operator=(DirectX::XMMATRIX matrix)
	{
		DirectX::XMStoreFloat4x4(&this->values, matrix);
	}
	Matrix4 operator*(Matrix4 otherMatrix)
	{
		return DirectX::XMMatrixMultiply(this->GetMatrix(), otherMatrix.GetMatrix());
	}
	Matrix4 operator*(DirectX::XMMATRIX otherMatrix)
	{
		return DirectX::XMMatrixMultiply(this->GetMatrix(), otherMatrix);
	}
	Matrix4 operator*(DirectX::XMFLOAT4X4 otherMatrix)
	{
		return DirectX::XMMatrixMultiply(this->GetMatrix(), XMLoadFloat4x4(&otherMatrix));
	}
	void operator*=(Matrix4 otherMatrix)
	{
		*this = *this * otherMatrix;
	}
	Matrix4 Transpose()
	{
		return DirectX::XMMatrixTranspose(this->GetMatrix());
	}
	Matrix4 Inverse(DirectX::XMVECTOR* determinant)
	{
		return DirectX::XMMatrixInverse(determinant, this->GetMatrix());
	}
	Matrix4 Inverse()
	{
		return DirectX::XMMatrixInverse(&this->Determinant().GetVector(), this->GetMatrix());
	}
	Matrix4 Translation(Vector3 position)
	{
		return *this * Matrix4(DirectX::XMMatrixTranslation(position.X(), position.Y(), position.Z()));
	}
	Matrix4 Rotation(Vector3 rotation)
	{
		return *this * DirectX::XMMatrixRotationQuaternion(DirectX::XMQuaternionRotationRollPitchYaw(
			DirectX::XMConvertToRadians(rotation.X()),
			DirectX::XMConvertToRadians(rotation.Y()),
			DirectX::XMConvertToRadians(rotation.Z()))) * this->GetMatrix();
	}
	Matrix4 Rotation(float z)
	{
		return *this * DirectX::XMMatrixRotationAxis(DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), z);
	}
	Matrix4 Scale(Vector3 scale)
	{
		return *this * Matrix4(DirectX::XMMatrixScaling(scale.X(), scale.Y(), scale.Z()));
	}
	Vector3 Determinant()
	{
		return DirectX::XMMatrixDeterminant(this->GetMatrix());
	}
};

// Transform contains the position rotation and scale data of an object.
// Also allows operation on data.
struct Transform
{
private:
	// Local transforms.
	Vector3 position;
	Vector3 rotation;
	Vector3 scale;

	// Stores the local axes of an object.
	Vector3 local_right;
	Vector3 local_up;
	Vector3 local_forward;

	void CalculateNewLocalAxes()
	{
		Matrix4 rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(DirectX::XMConvertToRadians(rotation.X()), DirectX::XMConvertToRadians(rotation.Y()), DirectX::XMConvertToRadians(rotation.Z()));
		this->local_right = DirectX::XMVector3Normalize(DirectX::XMVector3TransformCoord(global_right, rotationMatrix.GetMatrix()));
		this->local_up = DirectX::XMVector3Normalize(DirectX::XMVector3TransformCoord(global_up, rotationMatrix.GetMatrix()));
		this->local_forward = DirectX::XMVector3Normalize(DirectX::XMVector3TransformCoord(global_forward, rotationMatrix.GetMatrix()));
	}

public:
	Transform()
	{
		this->scale = Vector3(1.0f);
		this->CalculateNewLocalAxes();
	}

	// Get and Set functions.

	// Get the local position of the object.
	Vector3 GetPosition() { return this->position; }
	// Get the position of an object relative to its parent(if any).
	Vector3 GetGlobalPostion()
	{
		Vector3 resultPosition = this->position;
		if (this->hasParent)
		{
			resultPosition += parent->GetGlobalPostion();
		}
		return resultPosition;
	}
	void SetPosition(Vector3 position) { this->position = position; }
	void SetPosition(float x, float y, float z) { this->position = Vector3(x, y, z); }
	void SetPositionX(float x) { this->position = Vector3(x, this->position.Y(), this->position.Z()); }
	void SetPositionY(float y) { this->position = Vector3(this->position.X(), y, this->position.Z()); }
	void SetPositionZ(float z) { this->position = Vector3(this->position.X(), this->position.Y(), z); }
	void Translate(Vector3 translationVector)
	{
		this->position += translationVector;
	}

	// Get the local rotation of the object.(In Euler Angles)
	Vector3 GetRotation() { return this->rotation; }
	// Get the rotation of an object reletive to its parent(if any).
	Vector3 GetGlobalRotation()
	{
		Vector3 resultVector = this->rotation;
		if (this->hasParent)
		{
			resultVector += parent->GetGlobalRotation();
		}
		return resultVector;
	}
	void SetRotation(Vector3 rotation)
	{
		this->rotation = rotation;
		this->CalculateNewLocalAxes();
	}
	void SetRotation(float x, float y, float z)
	{
		this->rotation = Vector3(x, y, z);
		this->CalculateNewLocalAxes();
	}
	// Rotate in local X-axis(In Euler Angles).
	void RotateX(float rotateXDegrees)
	{
		this->rotation.X(this->rotation.X() + rotateXDegrees);
		this->CalculateNewLocalAxes();
	}
	// Rotate in local Y-axis(In Euler Angles).
	void RotateY(float rotateYDegrees)
	{
		this->rotation.Y(this->rotation.Y() + rotateYDegrees);
		this->CalculateNewLocalAxes();
	}
	// Rotate in local Z-axis(In Euler Angles).
	void RotateZ(float rotateZDegrees)
	{
		this->rotation.Z(this->rotation.Z() + rotateZDegrees);
		this->CalculateNewLocalAxes();
	}

	// Get the local scale of an object.
	Vector3 GetScale() { return this->scale; }
	// Get the scale of an object relative to its parent(if any).
	Vector3 GetGlobalScale()
	{
		Vector3 resultVector = this->scale;
		if (this->hasParent)
		{
			resultVector += parent->GetGlobalScale();
		}
		return resultVector;
	}
	void SetScale(Vector3 scale) { this->scale = scale; }
	void SetScale(float x, float y, float z) { this->scale = Vector3(x, y, z); }

	Vector3 GetLocalRight() { return this->local_right; }
	Vector3 GetLocalLeft() { return -this->local_right; }		// Negetive direction
	Vector3 GetLocalUp() { return this->local_up; }
	Vector3 GetLocalDown() { return -this->local_up; }			// Negetive direction
	Vector3 GetLocalForward() { return this->local_forward; }
	Vector3 GetLocalBackward() { return -this->local_forward; }	// Negetive direction

private:
	// Handling of parent transform relationship.
	bool hasParent = false;
	Transform* parent = nullptr;

public:
	// Function to set the transform parent.
	void SetParent(Transform* transform)
	{
		if (transform != nullptr)
		{
			this->parent = transform;
			this->hasParent = true;
		}

	}
	void RemoveParent()
	{
		this->parent = nullptr;
		this->hasParent = false;
	}
	bool HasParent() { return this->hasParent; }
	// Function gives currently set parent if any.
	Transform* GetParent()
	{
		Transform* transform = nullptr;
		if (this->hasParent)
		{
			transform = this->parent;
		}
		return transform;
	}

	// Function to get the model matrix relative to its parent.
	Matrix4 GetGlobalModel()
	{
		Matrix4 model;
		model = model.Translation(this->position);
		model = model.Rotation(this->rotation);
		model = model.Scale(this->scale);
		if (this->HasParent())
		{
			Matrix4 parentModel = parent->GetGlobalModel();
			model *= parentModel;
		}
		return model;
	}
	// Function to get the local model matrix.
	Matrix4 GetLocalModel()
	{
		Matrix4 model;
		model = model.Translation(this->position);
		model = model.Rotation(this->rotation);
		model = model.Scale(this->scale);
		return model;
	}
};

// Transform component for the 2D space.
struct Transform2D
{
private:
	// Local transforms.
	Vector2 position;
	float rotation = 0.0f;
	Vector2 scale;

	// Stores the local axes of an object.
	Vector2 local_up;
	Vector2 local_right;

	void CalculateNewLocalAxes()
	{
		Matrix4 rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(0.0f, 0.0f, DirectX::XMConvertToRadians(this->rotation));
		this->local_right = DirectX::XMVector3Normalize(DirectX::XMVector3TransformCoord(global_right, rotationMatrix.GetMatrix()));
		this->local_up = DirectX::XMVector3Normalize(DirectX::XMVector3TransformCoord(global_up, rotationMatrix.GetMatrix()));
	}

public:
	Transform2D()
	{
		this->scale = Vector2(1.0f);
		this->CalculateNewLocalAxes();
	}

	// Get and Set functions.

	// Get the local position of the object.
	Vector2 GetPosition() { return this->position; }
	// Get the position of an object relative to its parent(if any).
	Vector2 GetGlobalPostion()
	{
		Vector2 resultPosition = this->position;
		if (this->hasParent)
		{
			resultPosition += parent->GetGlobalPostion();
		}
		return resultPosition;
	}
	void SetPosition(Vector2 position) { this->position = position; }
	void SetPosition(float x, float y) { this->position = Vector2(x, y); }
	void SetPositionX(float x) { this->position = Vector2(x, this->position.Y()); }
	void SetPositionY(float y) { this->position = Vector2(this->position.X(), y); }
	void Translate(Vector2 translationVector)
	{
		this->position += translationVector;
	}

	// Get the local rotation of the object.(In Euler Angles)
	float GetRotation() { return this->rotation; }
	// Get the rotation of an object reletive to its parent(if any).
	float GetGlobalRotation()
	{
		float resultRotation = this->rotation;
		if (this->hasParent)
		{
			resultRotation += parent->GetGlobalRotation();
		}
		return resultRotation;
	}
	void SetRotation(float rotation)
	{
		this->rotation = rotation;
		this->CalculateNewLocalAxes();
	}
	// Rotate in local axis(in Degrees).
	void Rotate(float rotateDegrees)
	{
		this->rotation = this->rotation + rotateDegrees;
		this->CalculateNewLocalAxes();
	}

	// Get the local scale of an object.
	Vector2 GetScale() { return this->scale; }
	// Get the scale of an object relative to its parent(if any).
	Vector2 GetGlobalScale()
	{
		Vector2 resultVector = this->scale;
		if (this->hasParent)
		{
			resultVector += parent->GetGlobalScale();
		}
		return resultVector;
	}
	void SetScale(Vector2 scale) { this->scale = scale; }
	void SetScale(float x, float y) { this->scale = Vector2(x, y); }

	Vector2 GetLocalRight() { return this->local_right; }
	Vector2 GetLocalLeft() { return -this->local_right; }		// Negetive direction
	Vector2 GetLocalUp() { return this->local_up; }
	Vector2 GetLocalDown() { return -this->local_up; }			// Negetive direction

private:
	// Handling of parent transform relationship.
	bool hasParent = false;
	Transform2D* parent = nullptr;

public:
	// Function to set the transform parent.
	void SetParent(Transform2D* transform)
	{
		if (transform != nullptr)
		{
			this->parent = transform;
			this->hasParent = true;
		}

	}
	void RemoveParent()
	{
		this->parent = nullptr;
		this->hasParent = false;
	}
	bool HasParent() { return this->hasParent; }
	// Function gives currently set parent transform if any.
	Transform2D* GetParent()
	{
		Transform2D* transform = nullptr;
		if (this->hasParent)
		{
			transform = this->parent;
		}
		return transform;
	}

	// Function to get the model matrix relative to its parent.
	Matrix4 GetGlobalModel()
	{
		Matrix4 model;
		model = model.Scale(this->scale);
		model = model.Rotation(this->rotation);
		model = model.Translation(this->position);
		if (this->HasParent())
		{
			Matrix4 parentModel = parent->GetGlobalModel();
			model *= parentModel;
		}
		return model;
	}
	// Function to get the local model matrix.
	Matrix4 GetLocalModel()
	{
		Matrix4 model;
		model = model.Scale(this->scale);
		model = model.Rotation(this->rotation);
		model = model.Translation(this->position);
		return model;
	}
};