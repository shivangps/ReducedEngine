#include "MouseClass.h"

void MouseClass::OnLeftPressed(int x, int y)
{
	this->prevLeftIsDown = this->leftIsDown;
	this->leftIsDown = true;
	this->eventBuffer.push(MouseEvent(MouseEvent::EventType::LPress, x, y));
}

void MouseClass::OnLeftReleased(int x, int y)
{
	this->prevLeftIsDown = this->leftIsDown;
	this->leftIsDown = false;
	this->eventBuffer.push(MouseEvent(MouseEvent::EventType::LRelease, x, y));
}

void MouseClass::OnRightPressed(int x, int y)
{
	this->prevRightIsDown = this->rightIsDown;
	this->rightIsDown = true;
	this->eventBuffer.push(MouseEvent(MouseEvent::EventType::RPress, x, y));
}

void MouseClass::OnRightReleased(int x, int y)
{
	this->prevRightIsDown = this->rightIsDown;
	this->rightIsDown = false;
	this->eventBuffer.push(MouseEvent(MouseEvent::EventType::RRelease, x, y));
}

void MouseClass::OnMiddlePressed(int x, int y)
{
	this->prevMiddleIsDown = this->mButtonDown;
	this->mButtonDown = true;
	this->eventBuffer.push(MouseEvent(MouseEvent::EventType::MPress, x, y));
}

void MouseClass::OnMiddleReleased(int x, int y)
{
	this->prevMiddleIsDown = this->mButtonDown;
	this->mButtonDown = false;
	this->eventBuffer.push(MouseEvent(MouseEvent::EventType::MRelease, x, y));
}

void MouseClass::OnWheelUp(int x, int y)
{
	this->eventBuffer.push(MouseEvent(MouseEvent::EventType::WheelUp, x, y));
}

void MouseClass::OnWheelDown(int x, int y)
{
	this->eventBuffer.push(MouseEvent(MouseEvent::EventType::WheelDown, x, y));
}

void MouseClass::OnMouseMove(int x, int y)
{
	this->prevLeftIsDown = this->leftIsDown;
	this->prevRightIsDown = this->rightIsDown;
	this->prevMiddleIsDown = this->mButtonDown;
	this->x = x;
	this->y = y;
	this->eventBuffer.push(MouseEvent(MouseEvent::EventType::Move, x, y));
}

void MouseClass::OnMouseMoveRaw(int x, int y)
{
	this->eventBuffer.push(MouseEvent(MouseEvent::EventType::RAW_MOVE, x, y));
}

bool MouseClass::IsLeftDown()
{
	return this->leftIsDown;
}

bool MouseClass::IsMiddleDown()
{
	return this->mButtonDown;
}

bool MouseClass::IsRightDown()
{
	return this->rightIsDown;
}

bool MouseClass::IsLeftReleased()
{
	if (!this->leftIsDown && this->prevLeftIsDown)
		return true;
	return false;
}

bool MouseClass::IsRightReleased()
{
	if (!this->rightIsDown && this->prevRightIsDown)
		return true;
	return false;
}

bool MouseClass::IsMiddleReleased()
{
	if (!this->mButtonDown && this->prevMiddleIsDown)
		return true;
	return false;
}

int MouseClass::GetPosX()
{
	return this->x;
}

int MouseClass::GetPosY()
{
	return this->y;
}

MousePoint MouseClass::GetPos()
{
	return { this->x, this->y };
}

bool MouseClass::EventBufferIsEmpty()
{
	return this->eventBuffer.empty();
}

MouseEvent MouseClass::ReadEvent()
{
	if (this->eventBuffer.empty())
	{
		return MouseEvent();
	}
	else
	{
		MouseEvent e = this->eventBuffer.front();		//Get first event from the buffer
		this->eventBuffer.pop();				//Remove first event from the buffer
		return e;
	}
}
