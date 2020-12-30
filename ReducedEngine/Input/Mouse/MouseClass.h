#pragma once
#include"MouseEvent.h"
#include<queue>

// Class used to maintain a queue of events of mouse movements and clicks.
class MouseClass
{
public:
	void OnLeftPressed(int x, int y);
	void OnLeftReleased(int x, int y);
	void OnRightPressed(int x, int y);
	void OnRightReleased(int x, int y);
	void OnMiddlePressed(int x, int y);
	void OnMiddleReleased(int x, int y);
	void OnWheelUp(int x, int y);
	void OnWheelDown(int x, int y);
	void OnMouseMove(int x, int y);
	void OnMouseMoveRaw(int x, int y);

	bool IsLeftDown();
	bool IsMiddleDown();
	bool IsRightDown();
	bool IsLeftReleased();
	bool IsRightReleased();
	bool IsMiddleReleased();

	int GetPosX();
	int GetPosY();
	MousePoint GetPos();

	bool EventBufferIsEmpty();
	MouseEvent ReadEvent();

private:
	std::queue<MouseEvent> eventBuffer;
	bool leftIsDown = false;
	bool rightIsDown = false;
	bool mButtonDown = false;
	bool prevLeftIsDown = false;
	bool prevRightIsDown = false;
	bool prevMiddleIsDown = false;
	int x = 0;
	int y = 0;
};

