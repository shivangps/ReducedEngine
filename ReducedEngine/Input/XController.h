#pragma once

#include<Windows.h>
#include<Xinput.h>
#include<DirectXMath.h>

static const WORD XInput_Buttons[] =
{
	XINPUT_GAMEPAD_A,
	XINPUT_GAMEPAD_B,
	XINPUT_GAMEPAD_X,
	XINPUT_GAMEPAD_Y,
	XINPUT_GAMEPAD_DPAD_UP,
	XINPUT_GAMEPAD_DPAD_DOWN,
	XINPUT_GAMEPAD_DPAD_LEFT,
	XINPUT_GAMEPAD_DPAD_RIGHT,
	XINPUT_GAMEPAD_LEFT_SHOULDER,
	XINPUT_GAMEPAD_RIGHT_SHOULDER,
	XINPUT_GAMEPAD_LEFT_THUMB,
	XINPUT_GAMEPAD_RIGHT_THUMB,
	XINPUT_GAMEPAD_START,
	XINPUT_GAMEPAD_BACK
};

struct XInput_ButtonID {
	XInput_ButtonID();
public:
	int A, B, X, Y;
	int Dpad_Up, Dpad_Down, Dpad_Left, Dpad_Right;
	int L_Shoulder, R_Shoulder;
	int L_Thumbstick, R_Thumbstick;
	int Start;
	int Back;
};

// Class used for getting the inputs from the Xbox controller.
class XController
{
public:
	XController();
	XController(int pad_id);
	void UpdateGamepadState();

	//BUTTON Functions
	bool GetButtonPressed(int button);
	bool GetButtonDown(int button);

	//THUMBSTICK Functions
	//Left
	//Raw value of the magnitude for left thumbstick
	float GetLeftThumbMagnitude();
	//Normalized magnitude for left thumbstick
	float GetLeftThumbNormalizedMagnitude();
	//Returns boolean value if left thumbstick is in deadzone
	bool IsLeftThumbDeadzone();
	float GetLeftThumbX();
	float GetLeftThumbY();
	//Normalized X value of left thumbstick
	float GetLeftThumbNormalizedX();
	//Normalized Y value of left thumbstick
	float GetLeftThumbNormalizedY();
	//Right
	//Raw value of the magnitude for right thumbstick
	float GetRightThumbMagnitude();
	//Normalized magnitude for right thumbstick
	float GetRightThumbNormalizedMagnitude();
	//Returns boolean value if right thumbstick is in deadzone
	bool IsRightThumbDeadzone();
	float GetRightThumbX();
	float GetRightThumbY();
	//Normalized X value of right thumbstick
	float GetRightThumbNormalizedX();
	//Normalized Y value of right thumbstick
	float GetRightThumbNormalizedY();

	//Get the id of the gamepad.
	unsigned int GetPadID();

	//TRIGGER Functions
	//Left
	float GetTriggerL();
	//Normalized value for left trigger
	float GetNormalizedTriggerL();
	bool InDeadzoneTriggerL();
	//Right
	float GetTriggerR();
	//Normalized value for right trigger
	float GetNormalizedTriggerR();
	bool InDeadzoneTriggerR();
private:
	_XINPUT_GAMEPAD gamepadState;

	static const int button_count = 14;

	bool currentState[button_count];
	bool prevState[button_count];

	int padId = 0;

	//To store the magnitude of Left and Right Thumbsticks
	//Left
	float normalizedLX = 0.0f;
	float normalizedLY = 0.0f;
	float LX = 0.0f;
	float LY = 0.0f;
	float magnitude_L = 0.0f;
	float normalizedMagnitude_L = 0.0f;
	bool inDeadZone_L = true;
	//Right
	float normalizedRX = 0.0f;
	float normalizedRY = 0.0f;
	float RX = 0.0f;
	float RY = 0.0f;
	float magnitude_R = 0.0f;
	float normalizedMagnitude_R = 0.0f;
	bool inDeadZone_R = true;

	//Trigger button values
	BYTE trigger_L = (BYTE)0;
	float normalizedTriggerL = 0.0f;
	bool triggerInDeadzone_L = true;
	BYTE trigger_R = (BYTE)0;
	float normalizedTriggerR = 0.0f;
	bool triggerInDeadzone_R = true;
};