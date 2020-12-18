#pragma once

#include "Scene.h"
#include "Win32Handler.h"

// Game Engine class is a class that runs the whole game.
class GameEngine	// Single Instance
{
	GameEngine() 
	{
		this->time->Start();
	}

	// Current scene to run.
	Scene* currentScene = nullptr;

	// Win32 application class to run the display window.
	Win32Handler* win32Handler = Win32Handler::GetInstance();
	// Time instance.
	Time* time = Time::GetInstance();

public:
	static GameEngine* GetInstance()
	{
		static GameEngine* instance = new GameEngine();
		return instance;
	}

	// Function to set the scene to run.
	void SetScene(Scene* setNewScene)
	{
		this->currentScene = setNewScene;
	}

	// Game engine initialization.
	void Initialize(HINSTANCE hInstance);
	// Function to run the game in a continous non-stop loop for each frame.
	void RunGame();
};