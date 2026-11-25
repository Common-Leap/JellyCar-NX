#include "JellyGameManager.h"
#include "Game/JellySplash.h"
#include "Mods/ModLoader.h"

#include <sys/stat.h>

void JellyGameManager::Configure()
{
	//scan the JellyMods folder before anything loads levels, skins or songs
	mkdir("sdmc:/switch/JellyCar/JellyMods", 0777);
	ModLoader::Instance()->Init("sdmc:/switch/JellyCar/JellyMods");
}

void JellyGameManager::Init()
{
	exampleState = new JellySplash();
	//exampleState->Init();

	ChangeState(exampleState);
}

void JellyGameManager::CleanUp()
{
	//clean all states
	for (size_t i = 0; i < _states.size(); i++)
	{
		if (_states[i] != NULL)
		{
			_states[i]->CleanUp();
			delete _states[i];
		}
	}

	_states.clear();
}
