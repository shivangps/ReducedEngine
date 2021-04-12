#include "Output.h"

void Output::SetLeftRumble(unsigned int padId, float normalizedValue)
{
	// Calculate vibration intensity.
	int left_motor = int(normalizedValue * MAX_VIBRATION_VALUE);

	this->rumble.wLeftMotorSpeed = left_motor;

	// Apply vibration.
	XInputSetState(padId, &this->rumble);
}

void Output::SetRightRumble(unsigned int padId, float normalizedValue)
{
	// Calculate vibration intensity.
	int right_motor = int(normalizedValue * MAX_VIBRATION_VALUE);

	this->rumble.wRightMotorSpeed = right_motor;

	// Apply vibration.
	XInputSetState(padId, &this->rumble);
}
