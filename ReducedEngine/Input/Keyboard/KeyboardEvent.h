#pragma once

// Class used for recording a individual key character state(pressed or released) from the keyboard.
class KeyboardEvent
{
public:
	enum EventType
	{
		Press,
		Release,
		Invalid
	};

	KeyboardEvent();
	KeyboardEvent(const EventType type, const unsigned char key);
	bool IsPressed();
	bool IsRelease();
	bool IsValid();
	unsigned char GetKeyCode() const;

private:
	EventType type;
	unsigned char key;
};