#pragma once

#include "Scene.h"

// Game Engine class is a class that runs the whole game.
class GameEngine	// Single Instance
{
	GameEngine() {}

	// Current scene to run.
	Scene* currentScene = nullptr;

public:
	static GameEngine* GetInstance()
	{
		static GameEngine* instance = new GameEngine();
		return instance;
	}

	// Function to set the scene to run.
	void SetScene(Scene* setNewScene);

	// Function to run the game in a continous non-stop loop for each frame.
	void RunGame();
};