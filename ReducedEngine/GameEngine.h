#pragma once

#include "Scene.h"
#include "Win32Handler.h"
#include "Output/Graphics/Graphics.h"
#include "Output/Graphics/2D/Graphics2D.h"
#include "Output/Audio/Audio.h"

// Game Engine class is a class that runs the whole game.
class GameEngine	// Single Instance
{
	GameEngine() {}

	// Name of the window title.
	std::string window_title = "Reduced Engine";

	// Current scene to run.
	Scene* currentScene = NULL;

	// Win32 application class to run the display window.
	Win32Handler* win32Handler = Win32Handler::GetInstance();
	// Time instance.
	Time* time = Time::GetInstance();
	// Xbox Controller class.
	XController* controller = Input::GetInstance()->GetController();
	// Output class.
	Output* output = Output::GetInstance();
	// Graphics class.
#ifdef DX2D
	Graphics2D* graphics2d = Graphics2D::GetInstance();
#else
	Graphics* graphics = Graphics::GetInstance();
#endif // DX2D
	// Audio class.
	Audio* audio = Audio::GetInstance();

	// Function to be called to check to run the game.
	bool IsGameRunning();
	// Function to process all the inputs.
	void ProcessAllInputs();
	// Function to update all the states of game objects.
	void UpdateGameObjects();
	// Function to render all the game object render components present in the scene.
	void RenderScene();
	// Function to update physics objects in the scene.
	void UpdatePhysics();
	// Function to process all the sounds.
	void ProcessSounds();

public:
	static GameEngine* GetInstance()
	{
		static GameEngine* instance = new GameEngine();
		return instance;
	}

	// Function to set the scene to run.
	void SetScene(Scene* setNewScene);

	// Game engine initialization.
	void Initialize(HINSTANCE hInstance);
	// Function to run the game in a continous non-stop loop for each frame.
	void RunGame();

};