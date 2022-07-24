#pragma once
#include "../GameObject2D.h"
#include "../Output/Graphics/GUI/TextGUIComponent.h"

class PongPaddle;
class ScoreBorder;

// This object does management for the Pong game.
class PongGameManager : public GameObject2D
{
private:
	Transform2D p1ScoreTextTransform = Transform2D();
	Transform2D p2ScoreTextTransform = Transform2D();
	TextGUIComponent p1Text = TextGUIComponent(&p1ScoreTextTransform);
	TextGUIComponent p2Text = TextGUIComponent(&p2ScoreTextTransform);

	int p1Score = 0;
	int p2Score = 0;

public:
	PongGameManager(Transform2D transform)
	{
		this->objectTransform = transform;
	}

	// Function to initialize the game object.
	void Initialize(RenderList* sceneRenderComponentList, PhysicsComponentList* physicsComponentList, AudioComponentList* audioComponentList, GUIComponentList* guiComponentList);
	// Function call for updating the game object state.
	void Update();

	// Function to assign the pointer ot the score to respective border score.
	void AssignPlayerOneScore(ScoreBorder* p1Side);
	void AssignPlayerTwoScore(ScoreBorder* p2Side);
};