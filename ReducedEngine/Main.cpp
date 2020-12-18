#include "GameEngine.h"

// Main function.

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nCmdShow)
{
	Scene* scene = new Scene();

	scene->SetGameObject(new GameObject());

	GameEngine* mainEngine = GameEngine::GetInstance();

	mainEngine->Initialize(hInstance);

	mainEngine->SetScene(scene);

	mainEngine->RunGame();

	return 0;
}