#include "GameEngine.h"

void GameEngine::Initialize(HINSTANCE hInstance)
{
	// Initialize a new window.
	this->win32Handler->Initialize(hInstance, "Main Window", "EngineWindow");
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

	while (this->win32Handler->IsWindowOpen() && this->currentScene != nullptr)
	{
		// Process all the inputs.
		this->win32Handler->ProcessInput();

		unsigned int sceneObjectCount = this->currentScene->GetSize();
		// Process game objects.
		for (unsigned int i = 0; i < sceneObjectCount; i++)
		{
			GameObject* objectPointer = this->currentScene->GetGameObject(i);
			GameObject cachedObject = *objectPointer;

			cachedObject.Update();

			*objectPointer = cachedObject;
		}

		// Reset the frame time.
		this->time->ResetFrameTime();
	}
}
