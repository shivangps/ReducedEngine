#include "GameEngine.h"

#include "Assets/Scenes/MainScene.h"
#include "Assets/2D_Assets/Scenes/MainScene2D.h"
#include "Assets/2D_Assets/AtariPong/Scenes/PongScene.h"

// Main function.

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nCmdShow)
{
	GameEngine* mainEngine = GameEngine::GetInstance();

	mainEngine->Initialize(hInstance);

	Scene* scene = new MainScene();
#ifdef DX2D
	scene = new PongScene();
#endif // DX2D
	mainEngine->SetScene(scene);

	mainEngine->RunGame();

	return 0;
}