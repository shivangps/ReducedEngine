#pragma once

#include "CommonHeader.h"
#include "ErrorLogger.h"
#include "Time.h"
#include "Input/Input.h"
#include "Output/Output.h"

// This header file contains the convinient helper functions.

static void DebugString(LPCSTR lpcstr)
{
#ifdef _DEBUG
	OutputDebugStringA(lpcstr);
#endif // _DEBUG
	return;
}

// Function to remove the directory as well as the file extension from the file name.
static std::string GetFileNameFromDirectory(std::string fileDirectory)
{
	// Removing directory.
	size_t last_slash_idx = fileDirectory.find_last_of("\\");
	if (std::string::npos != last_slash_idx)
	{
		fileDirectory.erase(0, last_slash_idx + 1);
	}

	size_t last_forward_slash_idx = fileDirectory.find_last_of('/');
	if (std::string::npos != last_forward_slash_idx)
	{
		fileDirectory.erase(0, last_forward_slash_idx + 1);
	}

	// Removing extension.
	const size_t period_idx = fileDirectory.rfind('.');
	if (std::string::npos != period_idx)
	{
		fileDirectory.erase(period_idx);
	}

	return fileDirectory;
}

// Function to remove the reverse backslashes from the string and replace them with forward slash to conventional Directory string.
static std::string SanitizeBackSlashFileDirectory(std::string fileDirectory)
{
	for (unsigned int i = 0; i < fileDirectory.size(); i++)
	{
		if (fileDirectory[i] == '\\')
		{
			fileDirectory[i] = '/';

			while (fileDirectory[i + 1] == '\\')
			{
				fileDirectory.erase(i + 1, 1);
			}
		}
	}

	return fileDirectory;
}