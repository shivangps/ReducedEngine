#pragma once
#include "../GameObject2D.h"
#include "../Assets/2D_Assets/RenderComponents/WireframeRenderComponent2D.h"
#include "../Assets/2D_Assets/RenderComponents/SpriteRenderComponent2D.h"
#include "../Physics/2D/Colliders/BoxCollider2D.h"
#include "../Physics/2D/Rigidbody2D.h"

struct PaddleCharacteristics
{
private:
	const float XValue = 15.0f;		// Position of the paddle in x axis.
	const float medianVertical = 0.0f;		// Median distance that the paddle restores to.
	const float maxVerticalDistance = 9.0f;		// Maximum value in Y-axis for the paddle to traverse from the median distance.
	const float maxHorizontalDisatance = 15.0f;
	const float paddleSpeed = 4.0f;		// Speed at which the paddle traverses.
	const Vector2 scale = Vector2(0.5f, 2.0f);		// Scale for paddle.
	const std::string paddleTexture = "Assets/Textures/Paddles.png";

public:
	float GetXValue() { return XValue; }
	float GetMedianVertical() { return medianVertical; }
	float GetMaxVerticalDistance() { return maxVerticalDistance; }
	float GetMaxHorizontalDistance() { return maxHorizontalDisatance; }
	float GetPaddleSpeed() { return paddleSpeed; }
	Vector2 GetScale() { return scale; }
	std::string GetPaddleTexture() { return paddleTexture; }

	std::vector<MeshVertex2D> GetVertices()
	{
		std::vector<MeshVertex2D> vertices = {};

		vertices.push_back(MeshVertex2D(-0.5f, 0.5f, 0.0f, 0.0f));
		vertices.push_back(MeshVertex2D(-0.5f,-0.5f, 0.0f, 1.0f));
		vertices.push_back(MeshVertex2D( 0.5f, 0.5f, 1.0f, 0.0f));
		vertices.push_back(MeshVertex2D( 0.5f,-0.5f, 1.0f, 1.0f));

		return vertices;
	}
	std::vector<unsigned short> GetIndices()
	{
		std::vector<unsigned short> indices;

		indices.push_back(0);
		indices.push_back(1);
		indices.push_back(2);

		indices.push_back(1);
		indices.push_back(2);
		indices.push_back(3);

		return indices;
	}
};