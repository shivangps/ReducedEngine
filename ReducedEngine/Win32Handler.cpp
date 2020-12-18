#include "Win32Handler.h"

// Function to handle the windows input messages.
LRESULT CALLBACK HandleInput(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	Win32Handler* handler = Win32Handler::GetInstance();

	switch (uMsg)
	{
		// Keyboard messages.
	case WM_KEYDOWN:
	{
		// Exit the application if escape key is pressed.
		if (wParam == VK_ESCAPE)
		{
			handler->CloseWindow();
		}
	}
	return 0;
	// If close button is pressed then simply close the window.
	case WM_CLOSE:
	{
		handler->CloseWindow();
	}
	return 0;
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

}

void Win32Handler::CreateFullscreenMode()
{
	// Apply fullscreen.
	SetWindowLongPtr(this->hWnd, GWL_STYLE, WS_VISIBLE | WS_POPUP);
	SetWindowPos(this->hWnd, HWND_TOP, 0, 0, this->fullscreen_width, this->fullscreen_height, SWP_FRAMECHANGED);
	// Assign new current resolution to render.
	this->width = this->fullscreen_width;
	this->height = this->fullscreen_height;
}

void Win32Handler::Initialize(HINSTANCE hInstance, std::string window_title, std::string window_class)
{
	// Get the screen dimensions for fullscreen.
	this->fullscreen_width = GetSystemMetrics(SM_CXSCREEN);
	this->fullscreen_height = GetSystemMetrics(SM_CYSCREEN);

	// Initializing the window first in fullscreen mode.
	this->hInstance = hInstance;

	// Assign current resolution.
	this->width = this->fullscreen_width;
	this->height = this->fullscreen_height;

	this->window_class = window_class;
	this->window_class_wide = StringToWide(window_class);
	this->window_title = window_title;
	this->window_title_wide = StringToWide(window_title);

	WNDCLASSEX wc;

	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = HandleInput;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = this->hInstance;
	wc.hIcon = NULL;
	wc.hIconSm = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = this->window_class_wide.c_str();
	wc.cbSize = sizeof(WNDCLASSEX);

	if (!RegisterClassExW(&wc))
	{
		ErrorLog("Windows Registration Failed!");
		exit(-1);
	}

	this->hWnd = CreateWindowEx(
		0,
		this->window_class_wide.c_str(),
		this->window_title_wide.c_str(),
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_VISIBLE | WS_POPUP,
		0,
		0,
		this->width,
		this->height,
		NULL,
		NULL,
		this->hInstance,
		nullptr);

	// Sanity check for windows creation.
	if (this->hWnd == NULL)
	{
		ErrorLog("Windows creation failed for windows: " + window_title);
		exit(-1);
	}

	// Set the window handle to the global output class for DirectX12 API to access.
	Output::GetInstance()->SetHandle(this->hWnd);

	// Select the created window for current use in front of all windows opened.
	ShowWindow(this->hWnd, SW_SHOW);
	SetForegroundWindow(this->hWnd);
	SetFocus(this->hWnd);

	// Complete fullscreen mode.
	this->CreateFullscreenMode();

	return;
}

void Win32Handler::ProcessInput()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	if (PeekMessage(&msg, this->hWnd, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	if (msg.message == WM_NULL)
	{
		if (!IsWindow)
		{
			this->hWnd;
			this->openWindow = false;
			UnregisterClass(this->window_class_wide.c_str(), this->hInstance);
			return;
		}
	}
}

unsigned int Win32Handler::GetWidth()
{
	return this->width;
}

unsigned int Win32Handler::GetHeight()
{
	return this->height;
}

bool Win32Handler::IsWindowOpen()
{
	return this->openWindow;
}

void Win32Handler::CloseWindow()
{
	this->openWindow = false;
}
