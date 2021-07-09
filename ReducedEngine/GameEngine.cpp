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
	// Process the Xbox controller gamepad input.
	this->controller->UpdateGamepadState();
}

void GameEngine::UpdateGameObjects()
{
	if (this->currentScene)
	{
		// Get the number of game objects present in the currently assigned scene.
		unsigned int sceneObjectCount = this->currentScene->GetSize();
		// Update each game objects in the scene.
		for (unsigned int i = 0; i < sceneObjectCount; i++)
		{
			GameObject* objectPointer = this->currentScene->GetGameObject(i);

			objectPointer->Update();
		}
	}
}

void GameEngine::RenderScene()
{
	if (this->currentScene)
	{
		this->graphics->RenderScene(this->currentScene->GetRenderComponentList());
	}
}

void GameEngine::SetScene(Scene* setNewScene)
{
	this->currentScene = setNewScene;

	// Initialize the scene.
	this->graphics->InitializeRenderList(this->currentScene->GetRenderComponentList());
}

void GameEngine::Initialize(HINSTANCE hInstance)
{
	unsigned int window_width = 1280;
	unsigned int window_height = 720;
	// Initialize a new window.
	this->win32Handler->Initialize(hInstance, this->window_title, "EngineWindow", window_width, window_height);
	// Start the time for the game.
	this->time->Start();
	// Initialize the render engine.
	this->graphics->Initialize(this->output->GetHandle(), this->output->GetWindowWidth(), this->output->GetWindowHeight());

}

void GameEngine::RunGame()
{
	// SEQUENCE OF UPDATE PROCESS:
	// 1. Input
	// 2. Physics (Not present in this engine.)
	// 3. Game Objects state
	// 4. Sound (Not present in this engine.)
	// 5. Render

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
