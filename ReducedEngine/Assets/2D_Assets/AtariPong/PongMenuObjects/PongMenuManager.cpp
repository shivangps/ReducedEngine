#include "PongMenuManager.h"
#include "../Assets/2D_Assets/AtariPong/Scenes/PongScene.h"
#include "../Assets/2D_Assets/AtariPong/Objects/PongGameManager.h"
#include "PlayButtonObject.h"

void PongMenuManager::DisableMenu()
{
	this->playButtonObject->DisableGameObject();
	this->PONG_Text.DisableRendering();
	this->DisableGameObject();
}

void PongMenuManager::AssignPlayButtonObject(PlayButtonObject* playButtonObject)
{
	this->playButtonObject = playButtonObject;
}

void PongMenuManager::AssignGameManager(PongGameManager* gameManager)
{
	this->gameManager = gameManager;
}

void PongMenuManager::Initialize(RenderList* sceneRenderComponentList, PhysicsComponentList* physicsComponentList, AudioComponentList* audioComponentList, GUIComponentList* guiComponentList)
{
	// Show the cursor in the scene.
	Input::GetInstance()->GetMouse()->ShowMouseCursor(true);

	sceneRenderComponentList->SetBackgroundColor(this->backgroundColor);

	// Assign the text GUI component.
	guiComponentList->AssignGUIComponent(&this->PONG_Text);

	// Set the characteristics of the PONG text.
	this->PONG_Text.AssignText("PONG");
	this->PONG_Text.SetFont("Assets/Fonts/FOT-Rodin Pro UB.otf", 120);
	this->PONG_Text.SetColor(this->PONGTextColor);
	this->PONG_transform.SetPosition(0, 300);

	this->renderList = sceneRenderComponentList;

	//this->DisableMenu();
}

void PongMenuManager::Update()
{
	// If the mouse is clicked on the play button then change the scene to pong for playing.
	if (playButtonObject->GetClickState())
	{
		this->DisableMenu();
		this->gameManager->EnableGame();
	}
}

void PongMenuManager::EnableMenu()
{
	this->playButtonObject->EnableGameObject();
	this->PONG_Text.EnableRendering();
	this->EnableGameObject();
	this->renderList->SetBackgroundColor(this->backgroundColor);
}
