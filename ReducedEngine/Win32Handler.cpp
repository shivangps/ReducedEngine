#include "Win32Handler.h"

// Function to handle the windows input messages.(mainly mouse and keyboard)
LRESULT CALLBACK HandleInput(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	Output* output = Output::GetInstance();
	Input* input = Input::GetInstance();
	// Keyboard and Mouse instance.
	KeyboardClass* keyboard = input->GetKeyboard();
	MouseClass* mouse = input->GetMouse();

	switch (uMsg)
	{
	// Keyboard messages.

	// If the key is pressed down.
	case WM_KEYDOWN:
	{
		// Get the keycode pressed.
		unsigned char keycode = static_cast<unsigned char>(wParam);
		if (keyboard->IsKeyAutoRepeat())
		{
			keyboard->OnKeyPressed(keycode);
		}
		else
		{
			const bool wasPressed = lParam & 0x40000000;
			if (!wasPressed)
			{
				keyboard->OnKeyPressed(keycode);
			}
		}

		// Exit the application if escape key is pressed.
		if (wParam == VK_ESCAPE)
		{
			output->CloseWindow();
		}
	}
	return 0;
	// If the key is released.
	case WM_KEYUP:
	{
		unsigned char keycode = static_cast<unsigned char>(wParam);
		keyboard->OnKeyReleased(keycode);
	}
	return 0;

	// Mouse messages.

	// Mouse movement.
	case WM_MOUSEMOVE:
	{
		// Getting the mouse coordinates.
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);

		mouse->OnMouseMove(x, y);
	}
	return 0;
	// Left mouse button pressed.
	case WM_LBUTTONDOWN:
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);

		mouse->OnLeftPressed(x, y);
	}
	return 0;
	// Right mouse button pressed.
	case WM_RBUTTONDOWN:
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);

		mouse->OnRightPressed(x, y);
	}
	return 0;
	// Middle mouse button pressed.
	case WM_MBUTTONDOWN:
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);

		mouse->OnMiddlePressed(x, y);
	}
	return 0;
	// Left mouse button released.
	case WM_LBUTTONUP:
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);

		mouse->OnLeftReleased(x, y);
	}
	return 0;
	// Right mouse button released.
	case WM_RBUTTONUP:
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);

		mouse->OnRightReleased(x, y);
	}
	return 0;
	// Middle mouse button released.
	case WM_MBUTTONUP:
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);

		mouse->OnMiddleReleased(x, y);
	}
	return 0;
	// Mouse scroll wheel.
	case WM_MOUSEWHEEL:
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		if (GET_WHEEL_DELTA_WPARAM(wParam) > 0)
		{
			mouse->OnWheelUp(x, y);
		}
		else if (GET_WHEEL_DELTA_WPARAM(wParam) < 0)
		{
			mouse->OnWheelDown(x, y);
		}
	}
	return 0;
	// Get the raw mouse movement data input.
	case WM_INPUT:
	{
		UINT dataSize;

		GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, NULL, &dataSize, sizeof(RAWINPUTHEADER));

		if (dataSize > 0)
		{
			std::unique_ptr<BYTE[]> rawdata = std::make_unique<BYTE[]>(dataSize);
			if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, rawdata.get(), &dataSize, sizeof(RAWINPUTHEADER)) == dataSize)
			{
				RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(rawdata.get());
				if (raw->header.dwType == RIM_TYPEMOUSE && raw->data.mouse.lLastX != 0 || raw->data.mouse.lLastY != 0)
				{
					mouse->OnMouseMoveRaw(raw->data.mouse.lLastX, raw->data.mouse.lLastY);
				}
			}
		}

		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;

	// If close button is pressed then simply close the window.
	case WM_CLOSE:
	{
		output->CloseWindow();
	}
	return 0;
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

}

void Win32Handler::CreateFullscreenMode()
{
	// Assign new current resolution to render.
	this->width = this->fullscreen_width;
	this->height = this->fullscreen_height;
	// Recreate a new window.
	this->CreateNewWindow(&this->hWnd, 0, 0, this->width, this->height);
	// Apply fullscreen.
	SetWindowLongPtr(this->hWnd, GWL_STYLE, WS_VISIBLE | WS_POPUP);
	SetWindowPos(this->hWnd, HWND_TOP, 0, 0, this->fullscreen_width, this->fullscreen_height, SWP_FRAMECHANGED);

	this->fullscreenMode = true;
}

void Win32Handler::CreateWindowedMode()
{
	// Assign new current resolution to render.
	this->width = this->windowed_width;
	this->height = this->windowed_height;
	// Recreate a new window.
	this->CreateNewWindow(&this->hWnd, this->window_position_x, this->window_position_y, this->width, this->height);
	// Apply windowed.
	SetWindowLongPtr(this->hWnd, GWL_STYLE, WS_VISIBLE | WS_OVERLAPPEDWINDOW);
	SetWindowPos(this->hWnd, NULL, this->window_position_x, this->window_position_y, this->windowed_width, this->windowed_height, SWP_FRAMECHANGED);

	this->fullscreenMode = false;
}

bool Win32Handler::IsWindowedFullscreen()
{
	return this->fullscreenMode;
}

void Win32Handler::CreateNewWindow(HWND* hWnd, unsigned int window_offsetX, unsigned int window_offsetY, unsigned int width, unsigned int height)
{
	// First destroy the window.
	DestroyWindow(*hWnd);

	// Specify and create the window and get the window handle.
	this->hWnd = CreateWindowEx(
		0,
		this->window_class_wide.c_str(),
		this->window_title_wide.c_str(),
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_VISIBLE | WS_POPUP,
		window_offsetX,
		window_offsetY,
		width,
		height,
		NULL,
		NULL,
		this->hInstance,
		nullptr);

	// Sanity check for windows creation.
	if (*hWnd == NULL)
	{
		ErrorLog("Windows creation failed for windows: " + window_title);
		exit(-1);
	}

	// Set the window handle to the global output class for DirectX12 API to access.
	this->output->SetHandle(*hWnd);

	// Select the created window for current use in front of all windows opened.
	ShowWindow(*hWnd, SW_SHOW);
	SetForegroundWindow(*hWnd);
	SetFocus(*hWnd);
}

void Win32Handler::Initialize(HINSTANCE hInstance, std::string window_title, std::string window_class, unsigned int window_width, unsigned int window_height)
{
	// Get the screen dimensions (i.e screen resolution of monitor) for fullscreen.
	this->fullscreen_width = GetSystemMetrics(SM_CXSCREEN);
	this->fullscreen_height = GetSystemMetrics(SM_CYSCREEN);
	// Assign windowed resolution.
	this->windowed_width = window_width;
	this->windowed_height = window_height;

	this->hInstance = hInstance;

	// Initializing window class.
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

#ifndef _DEBUG
	// Complete fullscreen mode.
	this->CreateFullscreenMode();
#else
	// Complete windowed mode.
	this->CreateWindowedMode();
#endif // !_DEBUG

	// Assign the window resolution to the output class.
	this->output->SetWindowResoution(this->width, this->height);

	// Disable the cursor if it is disabled in output.
	if (!this->output->IsCursorEnabled())
	{
		RECT _clip;
		GetWindowRect(this->hWnd, &_clip);
		_clip.left += 5;
		_clip.top += 30;
		_clip.right -= 5;
		_clip.bottom -= 5;

		ClipCursor(&_clip);
		ShowCursor(FALSE);
	}

	return;
}

void Win32Handler::ProcessInput()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	while (PeekMessage(&msg, this->hWnd, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	if (msg.message == WM_NULL)
	{
		if (!IsWindow)
		{
			this->hWnd;
			this->output->CloseWindow();
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
