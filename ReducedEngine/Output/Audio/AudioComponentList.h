#pragma once
#include "AudioComponent.h"

// This class contains a list of all the audio components present in the scene,
// to be processed currently by the audio engine.

class AudioComponentList
{
private:
	std::vector<AudioComponent*> audioComponents = {};

public:
	// Function to set an audio component to the list.
	void AssignAudioComponent(AudioComponent* currentComponent);

	// Function to initlaize all the audio components present in the list.
	void InitializeAllAudioComponents(FMOD_RESULT* result, FMOD::System* system);

	// Function to update all the sudio components.
	void UpdateAllAudioComponents(FMOD_RESULT* result, FMOD::System* system);
};