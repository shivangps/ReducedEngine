#include "GameEngine.h"

#include "Assets/Scenes/MainScene.h"

// Main function.

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nCmdShow)
{
	GameEngine* mainEngine = GameEngine::GetInstance();

	mainEngine->Initialize(hInstance);

	mainEngine->SetScene(new MainScene());

	mainEngine->RunGame();

	return 0;
}