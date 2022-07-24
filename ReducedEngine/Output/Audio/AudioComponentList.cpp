#include "AudioComponentList.h"

void AudioComponentList::AssignAudioComponent(AudioComponent* currentAudioComponent)
{
	this->audioComponents.push_back(currentAudioComponent);
}

void AudioComponentList::InitializeAllAudioComponents(FMOD_RESULT* result, FMOD::System* system)
{
	for (unsigned int i = 0; i < this->audioComponents.size(); i++)
	{
		this->audioComponents[i]->InitializeComponent(system);
	}
}

void AudioComponentList::UpdateAllAudioComponents(FMOD_RESULT* result, FMOD::System* system)
{
	for (unsigned int i = 0; i < this->audioComponents.size(); i++)
	{
		this->audioComponents[i]->Update(system);
	}
}
