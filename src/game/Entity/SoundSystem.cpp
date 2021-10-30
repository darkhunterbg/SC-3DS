#include "SoundSystem.h"

#include "../Engine/AudioManager.h"
#include "../Game.h"

SoundSystem::SoundSystem()
{
	auto& channels = AudioManager::GetAudioChannels();

	_musicChannel.channel = Game::GetMusicChannel();


	for (const AudioChannelState& channel : channels)
	{
		if (&channel == Game::GetUIChannel())
			continue;

		if (channel.mono)
		{
			if (!_chatChannel.channel)
				_chatChannel.channel = &channel;
			else
				_worldChannels.push_back({ &channel });
		}
	}

}


void SoundSystem::PlayDef(const SoundSetDef& def, Channel& channel)
{
	AudioClip* clip;

	if (def.Randomize)
	{
		int next = _rand.Next(def.ClipCount);
		if (next == channel.soundIndex)
			next = (next + 1) % def.ClipCount;

		channel.soundIndex = next;
	}
	else
	{
		if (channel.sound != &def)
			channel.soundIndex = 0;
		else
		{
			channel.soundIndex = (channel.soundIndex + 1) % def.ClipCount;
		}
	}

	channel.sound = &def;

	clip = &def.GetAudioClip(channel.soundIndex);
	AudioManager::Play(*clip, channel.channel);
}


void SoundSystem::PlayMusic(const SoundSetDef& music)
{
	PlayDef(music, _musicChannel);
	//AudioManager::StopAll();
}

void SoundSystem::PlayChat(const SoundSetDef& chat)
{
	PlayDef(chat, _chatChannel);
}

void SoundSystem::DeleteEntities(std::vector<EntityId>& entities)
{
}

size_t SoundSystem::ReportMemoryUsage()
{
	return size_t();
}
