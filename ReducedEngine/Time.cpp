#include "Time.h"

void Time::CalculateFramesPerSecond()
{
	if (this->framesPerSecondTimer < 1.0f)
	{
		this->framesPerSecondTimer += (this->GetDeltaTime() / 1000.0f);
		this->framesPerSecondCounter++;
		this->changeFramesPerSecond = false;
	}
	else
	{
		this->framesPerSecond = this->framesPerSecondCounter;
		this->framesPerSecondTimer = this->framesPerSecondTimer - 1.0f;
		this->framesPerSecondCounter = 0;
		this->changeFramesPerSecond = true;
	}
}

double Time::GetCurrentStartTime()
{
	auto elapsed = std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - this->startTime);
	return elapsed.count();
}

void Time::Start()
{
	this->startTime = std::chrono::high_resolution_clock::now();
	this->stepTime = this->GetCurrentStartTime();
}

void Time::ResetFrameTime()
{
	this->prevStepTime = this->stepTime;
	this->stepTime = this->GetCurrentStartTime();
	this->CalculateFramesPerSecond();
}

float Time::GetDeltaTime()
{
	float deltaTime = this->stepTime - this->prevStepTime;
	return deltaTime;
}

unsigned int Time::GetFPS()
{
	return this->framesPerSecond;
}

bool Time::IsFPSChanged()
{
	return this->changeFramesPerSecond;
}
