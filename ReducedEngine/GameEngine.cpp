#include "GameEngine.h"

void GameEngine::RunGame()
{
	// SEQUENCE OF UPDATE PROCESS:
	// 1. Input
	// 2. Physics (Not present in this engine.)
	// 3. Game Objects state
	// 4. Render

	while (1 && this->currentScene != nullptr)
	{
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
