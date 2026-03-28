#include "AudioHelper.h"
#include <stdlib.h>
#include <time.h>

#include <sstream>
#include <Andromeda/Utils/Logger.h>
#include <Andromeda/FileSystem/FileManager.h>
#include "tinyxml.h"
#include "../Mods/ModLoader.h"

namespace AFS = Andromeda::FileSystem;

AudioHelper* AudioHelper::_instance = 0;

template <typename T>
std::string to_string(const T& value)
{
	std::ostringstream s;
	s << value;
	return s.str();
}

AudioHelper::AudioHelper()
{
	Init();
}

void AudioHelper::Init()
{
	_audioManager = AudioManager::Instance();

	//_carVolume = 0.0f;
	//_soundsVolume = 0.0f;
	//_musicVolume = 0.0f;

	_carVolume = 0.3f;
	_soundsVolume = 0.3f;
	_musicVolume = 0.1f;

	//try load settings
	LoadSetting();

	LoadSounds();
	LoadModSongs(); // append mod music tracks to playlist

	_musicPlaying = false;

	/* initialize random seed: */
	srand(time(NULL));
}

AudioHelper::~AudioHelper()
{
	Andromeda::Utils::Logger::Instance()->Log("AudioHelper::~AudioHelper begin\n");

	if (_musicPlaying && _music != 0)
	{
		_music->Stop();
		delete _music;
		_music = 0;
		_musicPlaying = false;
	}

	for (size_t i = 0; i < _hitSounds.size(); i++)
	{
		delete _hitSounds[i];
	}
	_hitSounds.clear();

	delete _carSlow;
	_carSlow = 0;
	delete _carFast;
	_carFast = 0;
	AudioManager::Shutdown();
	_audioManager = 0;

	Andromeda::Utils::Logger::Instance()->Log("AudioHelper::~AudioHelper done\n");
}

AudioHelper* AudioHelper::Instance()
{
	if (_instance == 0)
	{
		_instance = new AudioHelper();
	}

	return _instance;
}

void AudioHelper::Shutdown()
{
	if (_instance != 0)
	{
		Andromeda::Utils::Logger::Instance()->Log("AudioHelper::Shutdown begin\n");
		delete _instance;
		_instance = 0;
		Andromeda::Utils::Logger::Instance()->Log("AudioHelper::Shutdown done\n");
	}
}

void AudioHelper::LoadSounds()
{
	//load all hit sounds
	for (size_t i = 1; i < 14; i++)
	{
		std::string hitName = "hit" + to_string(i);

		Sound* sound = _audioManager->GetSound(hitName);
		sound->LoadWav("Assets/Jelly/Sounds/" + hitName + ".wav", false);
		sound->SetVolume(_soundsVolume);

		_hitSounds.push_back(sound);
	}

	//add background music file names — skip any disabled by disable.xml
	for (size_t i = 1; i < 8; i++)
	{
		if (ModLoader::Instance()->IsSongDisabled((int)i))
		{
			Andromeda::Utils::Logger::Instance()->Log("AudioHelper::LoadSounds: skipping built-in song %d (disabled)\n", (int)i);
			continue;
		}
		std::string songName = "Assets/Jelly/Music/song" + to_string(i) + ".ogg";
		_musicList.push_back(songName);
	}

	//load car engine sounds
	_carSlow = _audioManager->GetSound("carSlow");
	_carFast = _audioManager->GetSound("carFasts");

	_carSlow->LoadWav("Assets/Jelly/Sounds/car_low.wav", false);
	_carFast->LoadWav("Assets/Jelly/Sounds/car_high.wav", false);

	_carSlow->SetVolume(_carVolume);
	_carFast->SetVolume(_carVolume);

	_carSlow->SetLoop(true);
	_carFast->SetLoop(true);

	_music = 0;
}

void AudioHelper::PlayHitSound()
{
	if (_soundsVolume > 0.0f)
	{
		int soundNumber = rand() % 13;

		_hitSounds[soundNumber]->SetVolume(_soundsVolume);
		_hitSounds[soundNumber]->Play();
	}
}

void AudioHelper::PlayMusic()
{
	if (_musicVolume > 0.0f)
	{
		if (_musicPlaying)
		{
			if (_music != 0)
			{
				_music->Stop();
				delete _music;
				_musicPlaying = false;
			}
		}

		int soundNumber = rand() % (int)_musicList.size();

		Andromeda::Utils::Logger::Instance()->Log("AudioHelper::PlayMusic: picking '%s'\n", _musicList[soundNumber].c_str());

		_music = _audioManager->GetSound(_musicList[soundNumber]);
		if (!_music->LoadOgg(_musicList[soundNumber]))
		{
			Andromeda::Utils::Logger::Instance()->Log("AudioHelper::PlayMusic: LoadOgg FAILED for '%s'\n", _musicList[soundNumber].c_str());
		}
		_music->SetLoop(true);
		_music->SetVolume(_musicVolume);
		_music->Play();
		_musicPlaying = true;
	}
}

void AudioHelper::StopMusic()
{
	if (_musicPlaying)
	{
		_music->Stop();
		delete _music;
		_musicPlaying = false;
	}
}

void AudioHelper::PlayFastEngine()
{
	if (_carVolume > 0.0f)
	{
		_carSlow->Stop();
		_carFast->Play();
	}
}

void AudioHelper::PlaySlowEngine()
{
	if (_carVolume > 0.0f)
	{
		_carFast->Stop();
		_carSlow->Play();
	}
}

void AudioHelper::StopEngineSound()
{
	_carFast->Stop();
	_carSlow->Stop();
}

void AudioHelper::SetVolume(AudioHelperSoundEnum sound, float volume)
{
	switch (sound)
	{
		case Car:
		{
			if (_carVolume <= 0.0f && volume > _carVolume && !_carFast->IsPlaying())
			{
				_carFast->Play();
			}

			_carVolume = volume;

			_carFast->SetVolume(volume);
			_carSlow->SetVolume(volume);
		}
		break;
		case Sounds:
		{
			_soundsVolume = volume;

			for (size_t i = 0;i<_hitSounds.size();i++)
			{
				_hitSounds[i]->SetVolume(volume);
			}
		}
		break;
		case Music:
		{
			_musicVolume = volume;

			if (!_musicPlaying)
			{
				PlayMusic();
			}

			if (_music != NULL)
			{
				_music->SetVolume(volume);
			}
		}
		break;
	default: break;
	}
}

float AudioHelper::GetVolume(AudioHelperSoundEnum sound)
{
	switch (sound)
	{
	case Car:
	{
		return _carVolume;
	}
	break;
	case Sounds:
	{
		return _soundsVolume;
	}
	break;
	case Music:
	{
		return _musicVolume;
	}
	break;
	default: break;
	}

	return 0;
}

void AudioHelper::LoadSetting()
{
	std::string fileName = "JellyAudioSettings.xml";

	//loac main level file
	AFS::BaseFile* file = AFS::FileManager::Instance()->GetFile(fileName, true);

	if (file == 0)
		return;

	file->Open(AFS::Read, AFS::Binary);

	if (!file->Exist())
	{
		delete file;
		return;
	}


	int dataSize = 0;
	unsigned char* _buffer = file->GetData(dataSize);

	file->Close();
	delete file;

	int bodyNumber = 0;

	//load object
	TiXmlDocument doc;
	if (!doc.LoadContent(_buffer, dataSize))
	{
		return;
	}

	TiXmlHandle hDoc(&doc);
	TiXmlElement* pElem;
	TiXmlHandle hRoot(0);

	TiXmlElement* ObjectNode = pElem = hDoc.FirstChild("Settings").FirstChild().Element();
	for (ObjectNode; ObjectNode; ObjectNode = ObjectNode->NextSiblingElement())
	{
		std::string soundName = ObjectNode->Attribute("name");

		if (soundName == "Car")
		{
			_carVolume = atof(ObjectNode->Attribute("volume"));
		}
		else if (soundName == "Sounds")
		{
			_soundsVolume = atof(ObjectNode->Attribute("volume"));
		}
		else if (soundName == "Music")
		{
			_musicVolume = atof(ObjectNode->Attribute("volume"));
		}
	}
}

void AudioHelper::SaveSettings()
{
	std::string fileName = "JellyAudioSettings.xml";

	std::string saveFile = AFS::FileManager::Instance()->GetSaveDirPath() + fileName;

	TiXmlDocument doc;
	TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "", "");
	doc.LinkEndChild(decl);

	//root
	TiXmlElement * root = new TiXmlElement("Settings");
	doc.LinkEndChild(root);

	{
		TiXmlElement * cxn = new TiXmlElement("SoundLevel");
		root->LinkEndChild(cxn);
		cxn->SetAttribute("name", "Car");
		cxn->SetDoubleAttribute("volume", _carVolume);
	}

	{
		TiXmlElement * cxn = new TiXmlElement("SoundLevel");
		root->LinkEndChild(cxn);
		cxn->SetAttribute("name", "Sounds");
		cxn->SetDoubleAttribute("volume", _soundsVolume);
	}

	{
		TiXmlElement * cxn = new TiXmlElement("SoundLevel");
		root->LinkEndChild(cxn);
		cxn->SetAttribute("name", "Music");
		cxn->SetDoubleAttribute("volume", _musicVolume);
	}

	doc.SaveFile(saveFile.c_str());
}

void AudioHelper::LoadModSongs()
{
	const std::vector<ModSongInfo>& mods = ModLoader::Instance()->GetSongs();
	for (size_t i = 0; i < mods.size(); i++)
	{
		std::string fullPath = mods[i].modPath + "/" + mods[i].file;
		_musicList.push_back(fullPath);
		Andromeda::Utils::Logger::Instance()->Log("AudioHelper::LoadModSongs: added '%s'\n", fullPath.c_str());
	}
}
