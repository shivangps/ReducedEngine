#pragma once

#include "Helper.h"

// Class to handle the windows characteristics and getting keyboard and mouse inputs.

// It is a singleton class.
class Win32Handler
{
	Win32Handler() {}
	~Win32Handler() {}

public:
	static Win32Handler* GetInstance()
	{
		static Win32Handler* instance = new Win32Handler();
		return instance;
	}

private:
	HWND hWnd = NULL;
	HINSTANCE hInstance = NULL;
	std::string window_title = "";
	std::wstring window_title_wide = L"";
	std::string window_class = "";
	std::wstring window_class_wide = L"";
	// Current render resoution.
	unsigned int width = 100;
	unsigned int height = 100;
	// Resolution dimensions when windowed.
	unsigned int windowed_width = 100;
	unsigned int windowed_height = 100;
	// Resolution dimensions when fullscreen.
	unsigned int fullscreen_width = 100;
	unsigned int fullscreen_height = 100;
	// Position of windows.
	unsigned int window_position_x = 250;
	unsigned int window_position_y = 100;

	// To check if window is fullscreen or not.
	bool fullscreenMode = false;

	// Global Input class.
	Input* input = Input::GetInstance();
	// Global Output class.
	Output* output = Output::GetInstance();

	// Function to create a window.
	void CreateNewWindow(HWND* hWnd, unsigned int window_offsetX, unsigned int window_offsetY, unsigned int width, unsigned int height);

public:
	// Function to initialize the windows display and few other windows parameters such as windows title cursor and such...
	void Initialize(HINSTANCE hInstance, std::string window_title, std::string window_class, unsigned int window_width, unsigned int window_height);
	// Function to process the input message.
	void ProcessInput();
	// Get Functions for current width and height.
	unsigned int GetWidth();
	unsigned int GetHeight();

	// Function to create a fullscreen.
	void CreateFullscreenMode();
	// Function to create windowed screen.
	void CreateWindowedMode();
	// Get Function for fullscreen mode.
	bool IsWindowedFullscreen();
};