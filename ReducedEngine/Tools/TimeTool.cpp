#include "TimeTool.h"

void TimeTool::MarkTime()
{
#ifdef _DEBUG
	this->markTime = std::chrono::high_resolution_clock::now();
#endif // _DEBUG
}

void TimeTool::DisplayDeltaTime(std::string displayString)
{
#ifdef _DEBUG
	auto deltaTime = std::chrono::high_resolution_clock::now() - this->markTime;

	char numberString[50];
	sprintf_s(numberString, "%f", deltaTime);

	std::string printString = displayString + ": " + numberString + "\n";
	OutputDebugStringA(printString.c_str());

#endif // _DEBUG

}

void TimeTool::MarkAndDisplayTime(std::string markingString)
{
#ifdef _DEBUG
	auto deltaMarkTime = std::chrono::high_resolution_clock::now() - this->markTime;
	double deltaTime = std::chrono::duration<double, std::milli>(deltaMarkTime).count();

	char numberString[50];
	sprintf_s(numberString, "%f", deltaTime);

	std::string printString = markingString + ": " + numberString + "\n";
	OutputDebugStringA(printString.c_str());

	this->markTime = std::chrono::high_resolution_clock::now();
#endif // _DEBUG
}
