#include "Audio.h"

void Audio::INITSOUND()
{
	// Create system object and initialize.
	this->result = FMOD::System_Create(&this->system);

	this->result = this->system->init(32, FMOD_INIT_NORMAL, this->extradriverdata);

	this->result = this->system->createSound("Assets/SoundSamples/CardboardBox_Drop.wav", FMOD_DEFAULT, 0, &this->sound);

	this->result = this->sound->setMode(FMOD_LOOP_OFF);

	//this->result = this->system->playSound(this->sound, 0, false, &this->channel);

	//this->result = this->channel->setVolume(1.0f);
}

void Audio::TRYSOUND()
{
	this->result = this->system->update();

	unsigned int ms = 0; 
	unsigned int lenms = 0;
	bool         playing = 0;
	bool         paused = 0;
	int          channelsplaying = 0;

	/*
	if (this->channel)
	{
		FMOD::Sound* currentSound = 0;

		this->result = this->channel->isPlaying(&playing);

		this->result = this->channel->getPaused(&paused);

		this->result = this->channel->getPosition(&ms, FMOD_TIMEUNIT_MS);

		//this->channel->getCurrentSound(&currentSound);

		if (currentSound)
		{
			this->result = currentSound->getLength(&lenms, FMOD_TIMEUNIT_MS);
		}
	}
	*/
	this->system->getChannelsPlaying(&channelsplaying, NULL);
}

void Audio::Initilize()
{
	// Create system object and initialize.
	this->result = FMOD::System_Create(&this->system);
	ExitOnError(this->result);

	this->result = this->system->init(32, FMOD_INIT_NORMAL, this->extradriverdata);
	ExitOnError(this->result);
}

void Audio::InitalizeAudioComponentList(AudioComponentList* audioList)
{
	audioList->InitializeAllAudioComponents(&this->result, this->system);
}

void Audio::PlaySounds(AudioComponentList* audioList)
{
	audioList->UpdateAllAudioComponents(&this->result, this->system);

	this->result = this->system->update();
	ExitOnError(this->result);
}
