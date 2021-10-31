#include "SoundSystem.h"

#include "../Engine/AudioManager.h"
#include "EntityUtil.h"
#include "EntityManager.h"
#include "../Game.h"

static constexpr const int AnnoyedUnitChatCount = 5;

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

bool SoundSystem::IsPlayCompleted(Channel& channel)
{
	if (!_chatChannel.channel) return true;

	return _chatChannel.channel->IsDone();
}


void SoundSystem::PlayMusic(const SoundSetDef& music)
{
	PlayDef(music, _musicChannel);
	//AudioManager::StopAll();
}

void SoundSystem::PlayChat(const SoundSetDef& chat)
{
	_chatUnitId = Entity::None;
	_unitChatType = UnitChatType::None;
	_unitChatCount = 0;
	PlayDef(chat, _chatChannel);
}

bool SoundSystem::PlayUnitChat(EntityId id, UnitChatType type)
{
	auto& em = EntityUtil::GetManager();
	if (!em.UnitSystem.IsUnit(id)) return false;

	if (id == _chatUnitId && _unitChatType == type)
	{
		if (!IsPlayCompleted(_chatChannel)) return false;
	}
	else
	{
		_chatUnitId = id;
		_unitChatCount = 0;
	}

	if (_unitChatType != type)
	{
		_unitChatType = type;
		_unitChatCount = 0;
	}

	++_unitChatCount;

	const UnitDef* def = em.UnitSystem.GetComponent(id).def;

	int soundId = (int)_unitChatType;
	if (_unitChatType == UnitChatType::Selected && _unitChatCount > AnnoyedUnitChatCount)
	{
		if (def->Sounds.GetAnnoyedSound())
		{
			soundId++;
			if (_unitChatCount - AnnoyedUnitChatCount > def->Sounds.GetAnnoyedSound()->ClipCount)
			{
				_unitChatCount = 1;
				--soundId;
			}
		}
	}

	const SoundSetDef* chat = def->Sounds.GetSounds()[soundId];
	if (chat)
	{
		PlayDef(*chat, _chatChannel);
		return true;
	}

	return false;
}

void SoundSystem::DeleteEntities(std::vector<EntityId>& entities)
{
}

size_t SoundSystem::ReportMemoryUsage()
{
	return size_t();
}

void SoundSystem::UpdateSounds(const EntityManager& em)
{
	if (IsChatPlaying())
	{
		if (_chatUnitId != Entity::None && !em.HasEntity(_chatUnitId))
		{
			_chatUnitId = Entity::None;
			AudioManager::StopChannel(_chatChannel.channel);
		}
	}
}
