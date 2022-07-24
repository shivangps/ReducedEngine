#include "XController.h"

XController::XController()
{
	for (int i = 0; i < button_count; i++)
	{
		this->currentState[i] = false;
		this->prevState[i] = false;
	}
}

XController::XController(int pad_id)
{
	if (pad_id < 4 && pad_id >= 0)
	{
		this->padId = pad_id;
	}
	else if (pad_id >= 4)
	{
		this->padId = 3;
	}

	for (int i = 0; i < button_count; i++)
	{
		this->currentState[i] = false;
		this->prevState[i] = false;
	}
}

void XController::UpdateGamepadState()
{
	//To check if the controller is connected
	DWORD dwResult;

	XINPUT_STATE state;
	ZeroMemory(&state, sizeof(XINPUT_STATE));

	//Get the state of the input controller from 1 out of 4 possibly connected controllers. First being 0.
	dwResult = XInputGetState(padId, &state);
	this->gamepadState = state.Gamepad;

	memcpy(prevState, currentState, sizeof(prevState));

	if (dwResult == ERROR_SUCCESS)
	{
		//All buttons state update
		for (int i = 0; i < button_count; i++)
		{
			currentState[i] = (gamepadState.wButtons & XInput_Buttons[i]);
		}

		const long MAX_RADIUS = 32767;
		//LEFT THUMBSTICK calculation
		LX = gamepadState.sThumbLX;
		LY = gamepadState.sThumbLY;

		//Determine how far the controller is pushed
		magnitude_L = sqrt(LX * LX + LY * LY);

		//Determine the direction the controller is pushed in
		normalizedLX = (LX) / (MAX_RADIUS);
		normalizedLY = (LY) / (MAX_RADIUS);

		normalizedMagnitude_L = 0.0f;

		//Check if the controller is outside the circular deadzone
		if (magnitude_L > 0.0f)
		{
			//Clip the value at its expected maximum value
			if (magnitude_L > (float)MAX_RADIUS)
			{
				magnitude_L = MAX_RADIUS;
			}

			//Adjust magnitude relative to the end of the dead zone
			//magnitude_L -= XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;

			//Optionally normalize the magnitude
			normalizedMagnitude_L = magnitude_L / (float)(MAX_RADIUS);
			inDeadZone_L = false;
		}
		//If controller is in the deadzone
		else
		{
			magnitude_L = 0.0f;
			normalizedMagnitude_L = 0.0f;
			inDeadZone_L = true;
		}

		//RIGHT THUMBSTICK calculation
		float RX = gamepadState.sThumbRX;
		float RY = gamepadState.sThumbRY;

		//Detemine how far the thumbstick is pushed
		magnitude_R = sqrt(RX * RX + RY * RY);

		//Determine the direction in which the controller is pushed in relative to the right thumbstick deadzone.
		normalizedRX = (RX) / (MAX_RADIUS);
		normalizedRY = (RY) / (MAX_RADIUS);

		normalizedMagnitude_R = 0.0f;

		//Check if the controller is outside the circular deadzone
		if (magnitude_R > 0.0f)
		{
			//Clip the value at its expected maximum value
			if (magnitude_R > (float)MAX_RADIUS)
			{
				magnitude_R = MAX_RADIUS;
			}

			//Adjust magnitude relative to the end of the dead zone
			//magnitude_R -= XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;

			//Optionally normalize the magnitude
			normalizedMagnitude_R = magnitude_R / (float)(MAX_RADIUS);
			inDeadZone_R = false;
		}
		//If controller is in the deadzone
		else
		{
			magnitude_R = 0.0f;
			normalizedMagnitude_R = 0.0f;
			inDeadZone_R = true;
		}

		//LEFT TRIGGER
		trigger_L = gamepadState.bLeftTrigger;

		if (trigger_L > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
		{
			normalizedTriggerL = (trigger_L - XINPUT_GAMEPAD_TRIGGER_THRESHOLD) / 225.0f;
			triggerInDeadzone_L = false;
		}
		else
		{
			normalizedTriggerL = 0.0f;
			triggerInDeadzone_L = true;
		}

		//RIGHT TRIGGER
		trigger_R = gamepadState.bRightTrigger;

		if (trigger_R > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
		{
			normalizedTriggerR = (trigger_R - XINPUT_GAMEPAD_TRIGGER_THRESHOLD) / 225.0f;
			triggerInDeadzone_R = false;
		}
		else
		{
			normalizedTriggerR = 0.0f;
			triggerInDeadzone_R = true;
		}
	}
}

bool XController::GetButtonPressed(int button)
{
	return currentState[button];
}

bool XController::GetButtonDown(int button)
{
	return !prevState[button] && currentState[button];
}

float XController::GetLeftThumbMagnitude()
{
	return magnitude_L;
}

float XController::GetLeftThumbNormalizedMagnitude()
{
	return normalizedMagnitude_L;
}

bool XController::IsLeftThumbDeadzone()
{
	return inDeadZone_L;
}

float XController::GetLeftThumbX()
{
	return LX;
}

float XController::GetLeftThumbY()
{
	return LY;
}

float XController::GetLeftThumbNormalizedX()
{
	return normalizedLX;
}

float XController::GetLeftThumbNormalizedY()
{
	return normalizedLY;
}

float XController::GetRightThumbMagnitude()
{
	return magnitude_R;
}

float XController::GetRightThumbNormalizedMagnitude()
{
	return normalizedMagnitude_R;
}

bool XController::IsRightThumbDeadzone()
{
	return inDeadZone_R;
}

float XController::GetRightThumbX()
{
	return RX;
}

float XController::GetRightThumbY()
{
	return RY;
}

float XController::GetRightThumbNormalizedX()
{
	return normalizedRX;
}

float XController::GetRightThumbNormalizedY()
{
	return normalizedRY;
}

unsigned int XController::GetPadID()
{
	return this->padId;
}

float XController::GetTriggerL()
{
	return (float)trigger_L;
}

float XController::GetNormalizedTriggerL()
{
	return normalizedTriggerL;
}

bool XController::InDeadzoneTriggerL()
{
	return triggerInDeadzone_L;
}

float XController::GetTriggerR()
{
	return (float)trigger_R;
}

float XController::GetNormalizedTriggerR()
{
	return normalizedTriggerR;
}

bool XController::InDeadzoneTriggerR()
{
	return triggerInDeadzone_R;
}

XInput_ButtonID::XInput_ButtonID()
{
	A = 0;
	B = 1;
	X = 2;
	Y = 3;

	Dpad_Up = 4;
	Dpad_Down = 5;
	Dpad_Left = 6;
	Dpad_Right = 7;

	L_Shoulder = 8;
	R_Shoulder = 9;

	L_Thumbstick = 10;
	R_Thumbstick = 11;

	Start = 12;
	Back = 13;

}