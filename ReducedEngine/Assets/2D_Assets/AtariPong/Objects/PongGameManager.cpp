#include "PongGameManager.h"
#include "PongPaddle.h"
#include "PongBall.h"
#include "ScoreBorder.h"
#include "../Assets/2D_Assets/AtariPong/PongMenuObjects/PongMenuManager.h"

void PongGameManager::Initialize(RenderList* sceneRenderComponentList, PhysicsComponentList* physicsComponentList, AudioComponentList* audioComponentList, GUIComponentList* guiComponentList)
{
	guiComponentList->AssignGUIComponent(&this->p1Text);
	guiComponentList->AssignGUIComponent(&this->p2Text);

	this->p1Text.AssignText("0");
	this->p2Text.AssignText("0");
	this->p1Text.SetFont("Assets/Fonts/FOT-Rodin Pro UB.otf", 60);
	this->p2Text.SetFont("Assets/Fonts/FOT-Rodin Pro UB.otf", 60);

	this->p1ScoreTextTransform.SetPosition(Vector2( 600.0f, 400.0f));
	this->p2ScoreTextTransform.SetPosition(Vector2(-600.0f, 400.0f));

	//sceneRenderComponentList->SetBackgroundColor(this->backgroundColor);

	this->renderList = sceneRenderComponentList;

	this->DisableGame();
	//this->EnableGame();

	//this->p1Text.SetColor(Vector3(0.0f, 0.5f, 0.0f));
	//this->p2Text.SetColor(Vector3(0.5f, 0.0f, 0.0f));

	// Setting up texts for you win and you lost.
	guiComponentList->AssignGUIComponent(&this->youWinText);
	guiComponentList->AssignGUIComponent(&this->youLostText);

	this->youWinText.AssignText("You Win!!");
	this->youLostText.AssignText("You Lost");
	this->youWinText.SetFont("Assets/Fonts/FOT-Rodin Pro UB.otf", 60);
	this->youLostText.SetFont("Assets/Fonts/FOT-Rodin Pro UB.otf", 60);
	this->youWinText.SetColor(Vector3(0.1f, 0.7f, 0.1f));
	this->youLostText.SetColor(Vector3(0.7f, 0.1f, 0.1f));
	this->youWinText.enableText = false;
	this->youLostText.enableText = false;

	// Countdown Text initialization.
	guiComponentList->AssignGUIComponent(&this->countdownText);

	this->countdownTextTransform.SetPosition(26, 100);

	this->countdownText.AssignText("0");
	this->countdownText.SetFont("Assets/Fonts/FOT-Rodin Pro UB.otf", 60);
	this->countdownText.SetColor(Vector3(1.0f));
	this->countdownText.enableText = false;

	this->gameState = GameState::COUNTDOWNSTATE;
}

void PongGameManager::Update()
{

	XInput_ButtonID button_id;
	if (this->controller->GetButtonDown(button_id.B))
	{
		Output::GetInstance()->CloseWindow();
	}

	// If 'X' is pressed then go to menu.
	if (Input::GetInstance()->GetKeyboard()->KeyIsPressed('X'))
	{
		this->DisableGame();
		this->menuManager->EnableMenu();
	}

	int countdownNumber = 0;
	int roundCountdown = 0;

	// Execute based on game state.
	switch (this->gameState)
	{
	case GameState::COUNTDOWNSTATE:
		// Action.
		this->mouse->ShowMouseCursor(false);
		this->countdownText.enableText = true;
		this->pongBall->ResetPosition();
		countdownNumber = (int)(this->countDownDuration - this->countDownTimer);
		if (countdownNumber == 0)
		{
			this->countdownText.AssignText("Go!!");
			this->countdownText.SetColor(Vector3(0.1f, 0.7f, 0.1f));
		}
		else if (countdownNumber == 1)
		{
			this->countdownText.AssignText("Ready...");
			this->countdownText.SetColor(Vector3(0.7f, 0.7f, 0.1f));
		}
		else
		{
			this->countdownText.AssignText(std::to_string(countdownNumber));
			this->countdownText.SetColor(Vector3(0.7f, 0.1f, 0.1f));
		}
		// StateChange.
		if (this->countDownTimer > this->countDownDuration)
		{
			this->gameState = GameState::PLAYSTATE;
			this->countDownTimer = 0.0f;
			this->RestartGame();
			this->countdownText.enableText = false;
			this->roundState = RoundState::PLAYROUND;
		}
		else
		{
			this->countDownTimer += (this->time->GetDeltaTime() / 1000.0f);
		}
		break;
	case GameState::PLAYSTATE:
		// Action.
		this->p1Text.AssignText(std::to_string(this->p1Score));
		this->p2Text.AssignText(std::to_string(this->p2Score));

		// State for rounds.
		switch(this->roundState)
		{
		case RoundState::PLAYROUND:
			if (this->pongBall->IsOutofBounds() || (prevP1Score != p1Score || prevP2Score != p2Score))
			{
				this->roundState = RoundState::RESETROUND;
			}
			this->prevP1Score = p1Score;
			this->prevP2Score = p2Score;
			break;
		case RoundState::RESETROUND:
			if (this->roundResetTimer < this->roundResetDuration)
			{
				roundCountdown = (int)(this->roundResetDuration - this->roundResetTimer);
				this->countdownText.enableText = true;
				this->countdownText.AssignText(std::to_string(roundCountdown));
				this->countdownText.SetColor(Vector3(1.0f));
				this->roundResetTimer += (this->time->GetDeltaTime() / 1000.0f);
				this->pongBall->ResetPosition();
				this->pongBall->SetMovement(false);
			}
			else
			{
				this->countdownText.enableText = false;
				this->roundResetTimer = 0.0f;
				this->pongBall->ResetPosition();
				this->roundState = RoundState::PLAYROUND;
				this->pongBall->SetMovement(true);
			}
			break;
		}

		// StateChange.
		if (this->p1Score >= this->maxScore)
		{
			this->winCondition = WinCondition::WIN;
			this->gameState = GameState::GAMEOVER;
			this->DisableGameElements();

		}
		if (this->p2Score >= this->maxScore)
		{
			this->winCondition = WinCondition::LOST;
			this->gameState = GameState::GAMEOVER;
			this->DisableGameElements();
		}
		break;
	case GameState::GAMEOVER:
		// Action.
		if (this->winCondition == WinCondition::WIN)
		{
			this->youWinText.enableText = true;
		}
		else
		{
			this->youLostText.enableText = true;
		}
		// StateChange.
		if (this->winDisplayTimeKeep > this->winDisplayDuration)
		{
			this->gameState = GameState::COUNTDOWNSTATE;
			this->menuManager->EnableMenu();
			this->DisableGame();
			this->youLostText.enableText = false;
			this->youWinText.enableText = false;
			this->winDisplayTimeKeep = 0.0f;
		}
		else
		{
			this->winDisplayTimeKeep += (time->GetDeltaTime() / 1000.0f);
		}
		break;
	}
}

void PongGameManager::AssignPlayerOneScore(ScoreBorder* p1Side)
{
	p1Side->SetScore(&this->p1Score);
}

void PongGameManager::AssignPlayerTwoScore(ScoreBorder* p2Side)
{
	p2Side->SetScore(&this->p2Score);
}

void PongGameManager::AssignMenuManager(PongMenuManager* menuManager)
{
	this->menuManager = menuManager;
}

void PongGameManager::AssignGameSceneObjects(GameObject2D* gameObject)
{
	this->gameScenesObjects.push_back(gameObject);
}

void PongGameManager::AssignPongBall(PongBall* pongBall)
{
	this->pongBall = pongBall;
}

void PongGameManager::DisableGame()
{
	this->DisableGameElements();
	this->DisableGameObject();
}

void PongGameManager::DisableGameElements()
{
	for (unsigned int i = 0; i < this->gameScenesObjects.size(); i++)
	{
		this->gameScenesObjects[i]->DisableGameObject();
	}
	this->p1Text.enableText = false;
	this->p2Text.enableText = false;
}

void PongGameManager::EnableGame()
{
	this->EnableGameObject();
	this->renderList->SetBackgroundColor(this->backgroundColor);
	this->gameState = GameState::COUNTDOWNSTATE;
}

void PongGameManager::EnableGameElements()
{
	for (unsigned int i = 0; i < this->gameScenesObjects.size(); i++)
	{
		this->gameScenesObjects[i]->EnableGameObject();
	}
	this->p1Text.enableText = true;
	this->p2Text.enableText = true;
}

void PongGameManager::RestartGame()
{
	this->p1Score = 0;
	this->p2Score = 0;
	this->EnableGameElements();
	// Reset Pong Ball.
	Transform2D* pongBallTransform = this->pongBall->GetTransform2D();
	pongBallTransform->SetPosition(Vector2(0.0f));
}

void PongGameManager::ResetRound()
{
	this->roundState = RoundState::RESETROUND;
}
