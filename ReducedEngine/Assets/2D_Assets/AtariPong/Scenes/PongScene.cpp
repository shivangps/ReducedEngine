#include "PongScene.h"
#include "../Assets/2D_Assets/Objects/CameraObject2D/CameraObject2D.h"
#include "../Assets/2D_Assets/AtariPong/Objects/PlayerPaddle.h"
#include "../Assets/2D_Assets/AtariPong/Objects/EnemyPaddle.h"
#include "../Assets/2D_Assets/AtariPong/Objects/PongGameManager.h"
#include "../Assets/2D_Assets/AtariPong/Objects/PongBall.h"
#include "../Assets/2D_Assets/AtariPong/Objects/BorderObject.h"
#include "../Assets/2D_Assets/AtariPong/Objects/ScoreBorder.h"

PongScene::PongScene()
{
	GameObject2D* newGameobject2D = nullptr;
	Transform2D setTransform2d;

	// Camera Object 2D.
	setTransform2d = Transform2D();
	CameraObject2D* cameraObject = new CameraObject2D(setTransform2d);
	this->objects2DPresent.push_back(cameraObject);

	// Player pong paddle object.
	setTransform2d = Transform2D();
	PlayerPaddle* pongPlayerObject = new PlayerPaddle(setTransform2d);
	this->objects2DPresent.push_back(pongPlayerObject);

	// Set pong ball.
	setTransform2d = Transform2D();
	PongBall* pongBallObject = new PongBall(setTransform2d);
	this->objects2DPresent.push_back(pongBallObject);

	// Enemy pong paddle object.
	setTransform2d = Transform2D();
	EnemyPaddle* pongEnemyPaddle = new EnemyPaddle(setTransform2d);
	pongEnemyPaddle->AssignPongBall(pongBallObject);
	this->objects2DPresent.push_back(pongEnemyPaddle);

	// Set the border objects.
	setTransform2d = Transform2D();
	setTransform2d.SetPosition(Vector2(0.0f, 10.3f));
	BorderObject* borderObject1 = new BorderObject(setTransform2d);
	this->objects2DPresent.push_back(borderObject1);

	setTransform2d = Transform2D();
	setTransform2d.SetPosition(Vector2(0.0f, -10.3f));
	BorderObject* borderObject2 = new BorderObject(setTransform2d);
	this->objects2DPresent.push_back(borderObject2);

	// Set score border objects.
	setTransform2d = Transform2D();
	setTransform2d.SetPosition(-19.0f, 0.0f);
	ScoreBorder* scoreBorderObject1 = new ScoreBorder(setTransform2d);
	this->objects2DPresent.push_back(scoreBorderObject1);

	setTransform2d = Transform2D();
	setTransform2d.SetPosition(19.0f, 0.0f);
	ScoreBorder* scoreBorderObject2 = new ScoreBorder(setTransform2d);
	this->objects2DPresent.push_back(scoreBorderObject2);

	// Set the pong game manager.
	setTransform2d = Transform2D();
	PongGameManager* pongGameManagerObject = new PongGameManager(setTransform2d);
	pongGameManagerObject->AssignPlayerOneScore(scoreBorderObject1);
	pongGameManagerObject->AssignPlayerTwoScore(scoreBorderObject2);
	this->objects2DPresent.push_back(pongGameManagerObject);
}
