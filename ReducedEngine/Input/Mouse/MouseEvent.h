#pragma once

struct MousePoint
{
	int x;
	int y;
};

// Class used for recording single event of the mouse move or click.
class MouseEvent {
public:
	enum EventType {
		LPress,
		LRelease,
		RPress,
		RRelease,
		MPress,
		MRelease,
		WheelUp,
		WheelDown,
		Move,
		RAW_MOVE,
		Invalid
	};
private:
	EventType type;
	int x;
	int y;
public:
	MouseEvent();
	MouseEvent(const EventType type, const int x, const int y);
	bool IsValid() const;
	EventType GetType()	const;
	MousePoint GetPos() const;
	int GetPosY() const;
	int GetPosX() const;

};