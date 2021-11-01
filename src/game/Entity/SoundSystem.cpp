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

	_worldChannels.reserve(channels.Size() - 2);

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

	_channels.reserve(_worldChannels.size() + 2);
	_channels.push_back(&_musicChannel);
	_channels.push_back(&_chatChannel);
	for (auto& c : _worldChannels)
	{
		_channels.push_back(&c);
	}

}


void SoundSystem::PlayDef(const SoundSetDef& def, Channel& channel, float volume )
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
	channel.newSound = true;
	channel.volume = volume;
}

bool SoundSystem::IsPlayCompleted(Channel& channel)
{
	if (!_chatChannel.channel) return true;

	return _chatChannel.channel->IsDone();
}

void SoundSystem::PlayWorldSound(const SoundSetDef& sound, Vector2Int16 pos)
{
	_worldSounds.push_back({ pos, &sound, 0 });
}


void SoundSystem::PlayMusic(const SoundSetDef& music)
{
	PlayDef(music, _musicChannel);
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
	return _worldSounds.capacity() * sizeof(WorldSound);
}

void SoundSystem::UpdateSounds(const EntityManager& em, const Camera& camera)
{
	_soundsInRange.clear();

	Rectangle16 camRect = camera.GetRectangle16();
	Rectangle16 extendedCamRect = camRect;
	extendedCamRect.size = (extendedCamRect.size * 3) / 2;
	extendedCamRect.SetCenter(camRect.GetCenter());

	for (auto& s : _worldSounds)
	{
		if (extendedCamRect.Contains(s.position))
		{
			float volume = 0.5f + 0.5f * (camRect.Contains(s.position));
			s.volume = volume;
			_soundsInRange.push_back(s);
		};
	}

	_worldSounds.clear();

	for (WorldSound& s : _soundsInRange)
	{
		Channel* playChannel = nullptr;
		for (Channel& c : _worldChannels)
		{
			if (c.sound == s.sound)
			{
				playChannel = &c;
				break;
			}
		}

		if (playChannel == nullptr)
		{
			for (Channel& c : _worldChannels)
			{
				if (!IsPlayCompleted(c)) continue;
				playChannel = &c;
				break;
			}
		}

		if (playChannel)
			PlayDef(*s.sound, *playChannel, s.volume);
	}

	if (IsChatPlaying())
	{
		if (_chatUnitId != Entity::None && !em.HasEntity(_chatUnitId))
		{
			_chatUnitId = Entity::None;
			AudioManager::StopChannel(_chatChannel.channel);
		}
	}

	for (Channel* channel : _channels)
	{
		if (channel->newSound && channel->sound)
		{
			AudioClip& clip = channel->sound->GetAudioClip(channel->soundIndex);
			AudioManager::SetChannelVolume(channel->channel, channel->volume);
			AudioManager::Play(clip, channel->channel);
			channel->newSound = false;
		}
	}
}

