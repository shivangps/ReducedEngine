#include "GameEngine.h"

bool GameEngine::IsGameRunning()
{
	// Check if the window is open and the scene is assigned to run.
	return (this->currentScene != NULL && this->output->IsWindowOpen());
}

void GameEngine::ProcessAllInputs()
{
	// Process the inputs from keyboard and mouse.
	this->win32Handler->ProcessInput();
}

void GameEngine::UpdateGameObjects()
{
	// Get the number of game objects present in the currently assigned scene.
	unsigned int sceneObjectCount = this->currentScene->GetSize();
	// Update each game objects in the scene.
	for (unsigned int i = 0; i < sceneObjectCount; i++)
	{
		GameObject* objectPointer = this->currentScene->GetGameObject(i);
		// Using cache for quick calculation of each game object.
		GameObject cachedObject = *objectPointer;

		cachedObject.Update();

		*objectPointer = cachedObject;
	}
}

void GameEngine::RenderScene()
{
}

void GameEngine::Initialize(HINSTANCE hInstance)
{
	unsigned int window_width = 1280;
	unsigned int window_height = 720;
	// Initialize a new window.
	this->win32Handler->Initialize(hInstance, this->window_title, "EngineWindow", window_width, window_height);
	// Start the time for the game.
	this->time->Start();
}

void GameEngine::RunGame()
{
	// SEQUENCE OF UPDATE PROCESS:
	// 1. Input
	// 2. Physics (Not present in this engine.)
	// 3. Game Objects state
	// 4. Render

	// Main game loop running each frame.
	while (this->IsGameRunning())
	{
		// Process all the inputs.
		this->ProcessAllInputs();

		// Process all the game objects.
		this->UpdateGameObjects();

		// Render the game objects in the scene.
		this->RenderScene();

		// Reset the frame time.
		this->time->ResetFrameTime();

#ifdef _DEBUG
		// Update the FPS on the window title.
		if (this->time->IsFPSChanged())
		{
			std::string window_title_with_fps = this->window_title + " FPS: " + std::to_string(this->time->GetFPS());
			SetWindowTextA(Output::GetInstance()->GetHandle(), window_title_with_fps.c_str());
		}
#endif // !_DEBUG

	}
}
