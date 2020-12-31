#pragma once

#include "../ErrorLogger.h"
#include "Mouse/MouseClass.h"
#include "Keyboard/KeyboardClass.h"

// Class that handles the input via mouse, keyboard and Xbox controller.

// It is a singleton class.
class Input
{
	Input()
	{
		static bool raw_input_initialized = false;
		if (raw_input_initialized == false)
		{
			RAWINPUTDEVICE rid;

			rid.usUsagePage = 0x01;		//Mouse
			rid.usUsage = 0x02;
			rid.dwFlags = 0;
			rid.hwndTarget = NULL;

			if (RegisterRawInputDevices(&rid, 1, sizeof(rid)) == FALSE)
			{
				ErrorLog(GetLastError(), "Failed to register the raw input devices.");
				exit(-1);
				//registration failed. Call GetLastError for the cause of error
			}
			raw_input_initialized = true;
		}
	}

public:

	static Input* GetInstance()
	{
		static Input* instance = new Input();
		return instance;
	}
	KeyboardClass* GetKeyboard()
	{
		return &this->keyboard;
	}
	MouseClass* GetMouse()
	{
		return &this->mouse;
	}

private:
	KeyboardClass keyboard;
	MouseClass mouse;
};