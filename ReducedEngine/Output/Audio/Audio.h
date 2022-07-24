#pragma once
#include "AudioComponentList.h"

// This header file is responsible for the playback of sounds and audio.

class Audio
{
private:
	Audio() {}
public:
	// Single instance of this class.
	static Audio* GetInstance()
	{
		static Audio* instance = new Audio();
		return instance;
	}

public:
	// EXPERIMENTAL FUNCTION FOR TRY OUT.
	FMOD::System* system = nullptr;
	FMOD::Sound* sound = nullptr;
	FMOD_RESULT result;
	void* extradriverdata = 0;
	void INITSOUND();
	void TRYSOUND();

	// Function to initialize audio engine.
	void Initilize();
	// Function to initialize the audio component list.
	void InitalizeAudioComponentList(AudioComponentList* audioList);

	// Function to update all the render components.
	void PlaySounds(AudioComponentList* audioList);
};