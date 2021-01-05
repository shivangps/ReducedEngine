#pragma once

#include "CommonHeader.h"

// Structure to handle 3 float values namely X, Y and Z. Representing a vector.
struct Vector3
{
private:
	DirectX::XMFLOAT3 values = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);

	Vector3 Multiply(Vector3 vector, float aValue)
	{
		return Vector3(aValue * vector.X(), aValue * vector.Y(), aValue * vector.Z());
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
	Vector3 operator/(float aValue)
	{
		return this->Divide(*this, aValue);
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
		return DirectX::XMMatrixInverse(&this->Determinant(), this->GetMatrix());
	}
	Matrix4 Translation(Vector3 position)
	{
		return DirectX::XMMatrixTranslation(position.X(), position.Y(), position.Z()) * this->GetMatrix();
	}
	Matrix4 Rotation(Vector3 rotation)
	{
		return DirectX::XMMatrixRotationQuaternion(DirectX::XMQuaternionRotationRollPitchYaw(
			DirectX::XMConvertToRadians(rotation.X()),
			DirectX::XMConvertToRadians(rotation.Y()),
			DirectX::XMConvertToRadians(rotation.Z()))) * this->GetMatrix();
	}
	Matrix4 Scale(Vector3 scale)
	{
		return DirectX::XMMatrixScaling(scale.X(), scale.Y(), scale.Z()) * this->GetMatrix();
	}
	DirectX::XMVECTOR Determinant()
	{
		return DirectX::XMMatrixDeterminant(this->GetMatrix());
	}
};

// Constants that store the normalized values of 3 axes.
const DirectX::XMVECTOR global_right = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
const DirectX::XMVECTOR global_up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
const DirectX::XMVECTOR global_forward = DirectX::XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);

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
		Matrix4 rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(DirectX::XMConvertToRadians(rotation.X()), DirectX::XMConvertToRadians(rotation.Y()), 0.0f);
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

// Structure to store viewport rect.
struct Viewport
{
	D3D12_VIEWPORT values = {};

	Viewport()
	{
		this->values.MinDepth = 0.0f;
		this->values.MaxDepth = 1.0f;
		this->values.Width = 0;
		this->values.Height = 0;
		this->values.TopLeftX = 0.0f;
		this->values.TopLeftY = 0.0f;
	}
	void operator=(D3D12_VIEWPORT newViewport)
	{
		this->values = newViewport;
	}
};

// Structure to store rects.
struct Rect
{
	D3D12_RECT values = {};

	Rect()
	{
		this->values = { 0, 0, 0, 0 };
	}
	void operator=(D3D12_RECT newRect)
	{
		this->values = newRect;
	}
};