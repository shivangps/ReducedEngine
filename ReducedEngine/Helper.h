#pragma once

#include "CommonHeader.h"
#include "ErrorLogger.h"
#include "Time.h"
#include "Input/Input.h"
#include "Output.h"

// This header file contains the convinient helper functions.

static void DebugString(LPCSTR lpcstr)
{
#ifdef _DEBUG
	OutputDebugStringA(lpcstr);
#endif // _DEBUG
	return;
}