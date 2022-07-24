#pragma once
#include <fmod/fmod.hpp>
#include "../ErrorLogger.h"

// Contains the defination of the audio component class.

class AudioComponent
{
private:
	bool playSound = false;		// For triggering to play the sound.

	std::string fileLocation = "";		// To store the location of sound file.

	float volume = 1.0f;

	FMOD::Sound* sound = nullptr;

	FMOD::Channel* channel = nullptr;

public:
	AudioComponent() {}
	// Function calls for game developers.

	// Function to initialize the audio component.
	void Initialize(std::string soundFileLocation);
	// Function call for playing of sound.
	void Play();
	// Function to set the volume of the audio.
	void SetVolume(float volume);

public:
	// Function calls for engine.

	// Function to be used by the audio engine to initialize.
	void InitializeComponent(FMOD::System* system);
	// Function to call for update in the audio engine.
	void Update(FMOD::System* system);
};