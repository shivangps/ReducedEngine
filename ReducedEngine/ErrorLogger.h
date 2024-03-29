#pragma once

#include "CommonHeader.h"

static std::wstring StringToWide(std::string str)
{
	std::wstring wide_str(str.begin(), str.end());
	return wide_str;
}

static void DebugLog(std::string message)
{
#ifdef _DEBUG
	OutputDebugStringA(message.c_str());
#endif // _DEBUG
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

static void ExitOnError(HRESULT hr, std::string message)
{
#ifdef _DEBUG
	if (FAILED(hr))
	{
		ErrorLog(hr, message);
		exit(-1);
	}
#endif // DEBUG
}

static void ExitWithMessage(std::string message)
{
#ifdef _DEBUG
	ErrorLog(message);
#endif // _DEBUG
	exit(-1);
}

// For debugging fmod audio api.
#include <fmod/fmod_errors.h>

static void ExitOnError(FMOD_RESULT result)
{
#ifdef _DEBUG
	if (result != FMOD_OK)
	{
		std::string errorString = "ERROR (from FMOD): ";
		errorString = errorString + FMOD_ErrorString(result);
		DebugLog(errorString);
	}
#endif	// _DEBUG
}