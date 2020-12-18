#pragma once

#include <Windows.h>

// This file consist a singleton class that contains Handle for the output Window.

// It is a singleton class.
class Output
{
	Output() {}
	~Output() {}

	HWND windowHandle = NULL;

public:
	static Output* GetInstance()
	{
		static Output* instance = new Output();
		return instance;
	}

	HWND GetHandle() { return this->windowHandle; }
	void SetHandle(HWND windowHandle) { this->windowHandle = windowHandle; }
};