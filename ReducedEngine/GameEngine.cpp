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
#ifdef DX2D
		// Get the number of game objects present in the currently assigned scene.
		unsigned int sceneObject2DCount = this->currentScene->GetSize2D();
		// Update each game objects in the scene.
		for (unsigned int i = 0; i < sceneObject2DCount; i++)
		{
			GameObject2D* objectPointer = this->currentScene->GetGameObject2D(i);

			objectPointer->Update();
		}
#else
		// Get the number of game objects present in the currently assigned scene.
		unsigned int sceneObjectCount = this->currentScene->GetSize();
		// Update each game objects in the scene.
		for (unsigned int i = 0; i < sceneObjectCount; i++)
		{
			GameObject* objectPointer = this->currentScene->GetGameObject(i);

			objectPointer->Update();
		}
#endif // DX2D
	}
}

void GameEngine::RenderScene()
{
	if (this->currentScene)
	{
#ifdef DX2D
		this->graphics2d->RenderScene(this->currentScene->GetRenderComponentList(), this->currentScene->GetGUIComponentList());
#else
		this->graphics->RenderScene(this->currentScene->GetRenderComponentList());
#endif // DX2D

	}
}

void GameEngine::UpdatePhysics()
{
	this->currentScene->GetPhysicsComponentList()->Step(this->time->GetDeltaTime() / 1000.0f);
}

void GameEngine::ProcessSounds()
{
	this->audio->PlaySounds(this->currentScene->GetAudioComponentList());
}

void GameEngine::SetScene(Scene* setNewScene)
{
	this->currentScene = setNewScene;

	// Initialize the scene.

	// Game object initalization.
#ifdef DX2D
	// Get the number of game objects present in the currently assigned scene.
	unsigned int sceneObject2DCount = this->currentScene->GetSize2D();
	// Initalize each game objects in the scene.
	for (unsigned int i = 0; i < sceneObject2DCount; i++)
	{
		GameObject2D* objectPointer = this->currentScene->GetGameObject2D(i);

		objectPointer->Initialize(this->currentScene->GetRenderComponentList(), this->currentScene->GetPhysicsComponentList(), this->currentScene->GetAudioComponentList(), this->currentScene->GetGUIComponentList());
	}
#else
	// Get the number of game objects present in the currently assigned scene.
	unsigned int sceneObjectCount = this->currentScene->GetSize();
	// Update each game objects in the scene.
	for (unsigned int i = 0; i < sceneObjectCount; i++)
	{
		GameObject* objectPointer = this->currentScene->GetGameObject(i);

		objectPointer->Initialize(this->currentScene->GetRenderComponentList());
	}
#endif // DX2D


	// Render components initialization.
#ifdef DX2D
	this->graphics2d->InitializeRenderList(this->currentScene->GetRenderComponentList());

	this->graphics2d->InitializeGUIComponentList(this->currentScene->GetGUIComponentList());
#else
	this->graphics->InitializeRenderList(this->currentScene->GetRenderComponentList());
#endif // DX2D

	// Audio components initialization.
	this->audio->InitalizeAudioComponentList(this->currentScene->GetAudioComponentList());
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
#ifdef DX2D
	this->graphics2d->Initialize(this->output->GetHandle(), this->output->GetWindowWidth(), this->output->GetWindowHeight());
#else
	this->graphics->Initialize(this->output->GetHandle(), this->output->GetWindowWidth(), this->output->GetWindowHeight());
#endif // DX2D
	// Initlaize the sound engine.
	this->audio->Initilize();
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

		// Process physics.
		this->UpdatePhysics();

		// Render the game objects in the scene.
		this->RenderScene();

		// Reset the frame time.
		this->time->ResetFrameTime();

		// Process all the sounds.
		this->ProcessSounds();

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
