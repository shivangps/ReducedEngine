#include "AudioComponent.h"

void AudioComponent::Initialize(std::string soundFileLocation)
{
	this->fileLocation = soundFileLocation;
}

void AudioComponent::InitializeComponent(FMOD::System* system)
{
	FMOD_RESULT result;

	result = system->createSound(this->fileLocation.c_str(), FMOD_DEFAULT, 0, &this->sound);
	ExitOnError(result);

	result = this->sound->setMode(FMOD_LOOP_OFF);
	ExitOnError(result);
}

void AudioComponent::Play()
{
	this->playSound = true;
}

void AudioComponent::SetVolume(float volume)
{
	FMOD_RESULT result;

	this->volume = volume;
}

void AudioComponent::Update(FMOD::System* system)
{
	FMOD_RESULT result;			// For debuging.

	if (this->playSound && this->sound)
	{
		result = system->playSound(this->sound, NULL, false, &channel);
		ExitOnError(result);

		result = this->channel->setVolume(this->volume);
		ExitOnError(result);

		playSound = false;
	}
}
