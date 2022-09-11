#pragma once
#include "../GameObject2D.h"
#include "../Output/Graphics/GUI/TextGUIComponent.h"

class PongPaddle;
class ScoreBorder;

// This object does management for the Pong game.
class PongGameManager : public GameObject2D
{
private:
	Time* time = Time::GetInstance();

	XController* controller = Input::GetInstance()->GetController();
	MouseClass* mouse = Input::GetInstance()->GetMouse();

	Transform2D p1ScoreTextTransform = Transform2D();
	Transform2D p2ScoreTextTransform = Transform2D();
	TextGUIComponent p1Text = TextGUIComponent(&p1ScoreTextTransform);
	TextGUIComponent p2Text = TextGUIComponent(&p2ScoreTextTransform);

	Transform2D youWinTextTransform = Transform2D();
	Transform2D youLostTextTransform = Transform2D();
	TextGUIComponent youWinText = TextGUIComponent(&youWinTextTransform);
	TextGUIComponent youLostText = TextGUIComponent(&youLostTextTransform);
	float winDisplayDuration = 3.0f;
	float winDisplayTimeKeep = 0.0f;

	class PongMenuManager* menuManager = nullptr;

	class PongBall* pongBall = nullptr;

	int prevP1Score = 0;
	int prevP2Score = 0;
	int p1Score = 0;
	int p2Score = 0;
	int maxScore = 5;

	std::vector<GameObject2D*> gameScenesObjects = {};

	bool isGameEnabled = false;

	RenderList* renderList = nullptr;

	Vector3 backgroundColor = Vector3(0.05f, 0.05f, 0.5f);

	enum class RoundState
	{
		RESETROUND = 0,
		PLAYROUND,
	};
	RoundState roundState = RoundState::PLAYROUND;
	enum class GameState
	{
		COUNTDOWNSTATE = 0,
		PLAYSTATE,
		GAMEOVER,
	};
	GameState gameState = GameState::COUNTDOWNSTATE;
	enum class WinCondition
	{
		WIN = 0,
		LOST
	}winCondition;
	bool roundOver = false;
	bool gameOver = false;

	float countDownDuration = 4.0f;
	float countDownTimer = 0.0f;
	Transform2D countdownTextTransform = Transform2D();
	TextGUIComponent countdownText = TextGUIComponent(&this->countdownTextTransform);

	float roundResetDuration = 3.0f;
	float roundResetTimer = 0.0f;


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

	// Function to assign the menu manager.
	void AssignMenuManager(PongMenuManager* menuManager);
	// Function to assign the game scene objects.
	void AssignGameSceneObjects(GameObject2D* gameObject);
	// Function to assign the pong ball.
	void AssignPongBall(PongBall* pongBall);
	// Function to disable the game.
	void DisableGame();
	// Function to disable the game elements.
	void DisableGameElements();
	// Function to enable the game.
	void EnableGame();
	// Function to enable the game elements.
	void EnableGameElements();
	// Function to restart the game.
	void RestartGame();
	// Function to reset the round.
	void ResetRound();
};