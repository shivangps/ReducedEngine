#pragma once
#include <string>
#include <chrono>
#include "../CommonHeader.h"

// This header function contains the tool for handling time stamps.

class TimeTool
{
private:
	std::chrono::time_point<std::chrono::steady_clock> markTime = std::chrono::high_resolution_clock::now();

public:
	// Function to mark the time.
	void MarkTime();
	// Function to display the delta of previous marked time.
	void DisplayDeltaTime(std::string displayString);
	// Function to display the delta of previous marked time and mark the new time.
	void MarkAndDisplayTime(std::string markingString);
};