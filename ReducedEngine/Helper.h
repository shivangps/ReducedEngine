#pragma once

#include "CommonHeader.h"
#include <comdef.h>

// This header file contains the convinient helper functions.

static std::wstring StringToWide(std::string str)
{
	std::wstring wide_str(str.begin(), str.end());
	return wide_str;
}

static void ErrorLog(std::string message)
{
#ifdef _DEBUG
	std::string error_message = "Error: " + message;
	MessageBoxA(NULL, error_message.c_str(), "Error: ", MB_ICONERROR);
#endif // DEBUG
}

static void ErrorLog(HRESULT hr, std::string message)
{
#ifdef _DEBUG
	_com_error error(hr);
	std::wstring error_message = L"Error: " + StringToWide(message) + L"\n" + error.ErrorMessage();
	MessageBoxW(NULL, error_message.c_str(), L"Error: ", MB_ICONERROR);
#endif // DEBUG
}