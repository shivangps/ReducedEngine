#pragma once

#include <Windows.h>

// This file consist a singleton class that contains Handle for the output Window.

// It is a singleton class.
class Output
{
	Output() {}
	~Output() {}

	// Window handle.
	HWND windowHandle = NULL;
	// Window resolution.
	unsigned int window_width = 100;
	unsigned int window_height = 100;
	// Boolean to check if window is open.
	bool openWindow = true;

public:
	static Output* GetInstance()
	{
		static Output* instance = new Output();
		return instance;
	}

	HWND GetHandle() { return this->windowHandle; }
	void SetHandle(HWND windowHandle) { this->windowHandle = windowHandle; }

	// Function to get and set the resolution of the screen.
	void SetWindowResoution(unsigned int width, unsigned int height) { this->window_width = width, this->window_height = height; }
	unsigned int GetWindowWidth() { return this->window_width; }
	unsigned int GetWindowHeight() { return this->window_height; }

	// Function to close the window.
	void CloseWindow() { this->openWindow = false; }
	// Fucntion to check if the window is to be left open.
	bool IsWindowOpen() { return this->openWindow; }
};