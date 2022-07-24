#include "PongGameManager.h"
#include "PongPaddle.h"
#include "ScoreBorder.h"

void PongGameManager::Initialize(RenderList* sceneRenderComponentList, PhysicsComponentList* physicsComponentList, AudioComponentList* audioComponentList, GUIComponentList* guiComponentList)
{
	guiComponentList->AssignGUIComponent(&this->p1Text);
	guiComponentList->AssignGUIComponent(&this->p2Text);

	this->p1Text.AssignText("00");
	this->p2Text.AssignText("00");
	this->p1Text.SetFont("Assets/Fonts/FOT-Rodin Pro UB.otf", 60);
	this->p2Text.SetFont("Assets/Fonts/FOT-Rodin Pro UB.otf", 60);

	this->p1ScoreTextTransform.SetPosition(Vector2( 600.0f, 400.0f));
	this->p2ScoreTextTransform.SetPosition(Vector2(-600.0f, 400.0f));

	//this->p1Text.SetColor(Vector3(0.0f, 0.5f, 0.0f));
	//this->p2Text.SetColor(Vector3(0.5f, 0.0f, 0.0f));
}

void PongGameManager::Update()
{
	this->p1Text.AssignText(std::to_string(this->p1Score));
	this->p2Text.AssignText(std::to_string(this->p2Score));
}

void PongGameManager::AssignPlayerOneScore(ScoreBorder* p1Side)
{
	p1Side->SetScore(&this->p1Score);
}

void PongGameManager::AssignPlayerTwoScore(ScoreBorder* p2Side)
{
	p2Side->SetScore(&this->p2Score);
}
