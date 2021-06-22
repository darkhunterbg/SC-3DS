#include "SoundSystem.h"

#include "../Camera.h"
#include "EntityManager.h"
#include "../Profiler.h"

#include "../Data/RaceDef.h"
#include "../MathLib.h"
#include "../Data/SoundSetDef.h"
#include "../Data/UnitDef.h"

#include "../Engine/AudioManager.h"

#include <algorithm>

static constexpr const int PreAnnoyedChatCount = 5;


SoundSystem::SoundSystem()
{
	auto& channels = AudioManager::GetAudioChannes();

	const auto worldChannels = Span<AudioChannelState>(channels.Data(), channels.Size() - 2);

	for (const auto& channel : worldChannels) {
		if (channel.mono) {
			EntityAudioChannel eac;
			eac.channel = &channel;
			worldAudioChannels.push_back(eac);
		}
	}

	chatAudioChannel.channel = &channels[channels.Size() - 2];
	uiAudioChannel.channel = &channels[channels.Size() - 1];

	rnd = Random(std::rand());
}

bool SoundSystem::EntityAudioSort(const EntityPriorityAudio& a, EntityPriorityAudio& b) {
	return a.priority > b.priority;
}

void SoundSystem::CollectAudioFromSources(const Camera& camera, EntityManager& em) {
	playWorldAudio.clear();
	entityUniqueAudio.clear();
	entityAudioPriority.clear();

	Rectangle16 camRect = camera.GetRectangle16();
	Rectangle16 extendedCamRect = camRect;
	extendedCamRect.size = (extendedCamRect.size * 3) / 2;
	extendedCamRect.SetCenter(camRect.GetCenter());

	for (EntityId id : em.SoundArchetype.Archetype.GetEntities()) {

		auto& flags = em.FlagComponents.GetComponent(id);
		if (!flags.test(ComponentFlags::SoundTrigger))
			continue;

		flags.clear(ComponentFlags::SoundTrigger);

		if (flags.test(ComponentFlags::SoundMuted))
			continue;

		const SoundSourceComponent& src = em.SoundArchetype.SourceComponents.GetComponent(id);

		if (src.clip->id == 0)
			continue;

		const Vector2Int16& pos = em.PositionComponents.GetComponent(id);

		if (extendedCamRect.Contains(pos)) {

			float isFullAudio = camRect.Contains(pos);

			int foundIndex = -1;

			int i = 0;
			for (const auto& clip : entityUniqueAudio) {
				if (clip->id == src.clip->id)
				{
					foundIndex = i;
					break;
				}
				++i;
			}

			if (foundIndex < 0) {
				entityUniqueAudio.push_back(src.clip);
				uint16_t priority = src.priority;
				float volume = isFullAudio ? 1.0f : 0.5f;
				entityAudioPriority.push_back({ priority,  (uint16_t)entityAudioPriority.size(), volume });
			}
			else if (isFullAudio) {

				entityAudioPriority[foundIndex].priority = src.priority;
				entityAudioPriority[foundIndex].volume = 1.0f;
			}
		}
	}

	std::sort(entityAudioPriority.begin(), entityAudioPriority.end(), EntityAudioSort);

	int max = std::min(entityUniqueAudio.size(), worldAudioChannels.size());

	for (int i = 0; i < max; ++i) {
		int index = entityAudioPriority[i].clipIndex;
		AudioClip* clip = entityUniqueAudio[index];
		uint16_t priority = entityAudioPriority[i].priority;
		float volume = entityAudioPriority[i].volume;

		playWorldAudio.push_back({ clip,volume, priority });
	}

}

void SoundSystem::UpdateEntityAudio(const Camera& camera, EntityManager& em)
{
	CollectAudioFromSources(camera, em);

	int max = std::min(playWorldAudio.size(), worldAudioChannels.size());

	for (int j = 0; j < worldAudioChannels.size(); ++j) {
		worldAudioChannels[j].queued = false;
	}

	for (int i = 0; i < max; ++i) {

		auto& clip = playWorldAudio[i].clip;

		bool channelFound = false;

		for (int j = 0; j < worldAudioChannels.size(); ++j) {
			auto& channel = worldAudioChannels[j];
			if (channel.queued)
				continue;

			if (channel.channel->IsDone() || channel.clipId == clip->id)
			{
				AudioManager::SetChannelVolume(channel.channel->ChannelId, playWorldAudio[i].volume);
				AudioManager::PlayClip(clip, channel.channel->ChannelId);
				channel.clipId = clip->id;
				channel.queued = true;
				channel.clipPriority = playWorldAudio[i].priority;
				channelFound = true;
				break;
			}
		}

		if (!channelFound) {
			// Failed to find free channel, we'll override one with lowest priortiy
			uint16_t prio = playWorldAudio[i].priority;
			int channelId = -1;

			for (int j = 0; j < worldAudioChannels.size() - 1; ++j) {
				auto& channel = worldAudioChannels[j];
				if (channel.queued)
					continue;

				if (channel.clipPriority < prio) {
					channelId = j;
					prio = channel.clipPriority;
				}

			}

			if (channelId != -1) {
				channelFound = true;
				auto& channel = worldAudioChannels[channelId];

				AudioManager::SetChannelVolume(channel.channel->ChannelId, playWorldAudio[i].volume);
				AudioManager::PlayClip(clip, channel.channel->ChannelId);
				channel.clipId = clip->id;
				channel.queued = true;
				channel.clipPriority = playWorldAudio[i].priority;
				channelFound = true;
			}
		}
	}


}


void SoundSystem::PlayUnitReady(EntityId id, const UnitDef& unit) {
	newChatRequest = { unit.Sounds.GetReadySound(), id };
}
void SoundSystem::PlayUnitSelect(EntityId id, const UnitDef& unit) {

	if (currentChat.id == id) {
		if (unit.Sounds.GetAnnoyedSound()) {
			int annoyedCount = unit.Sounds.GetAnnoyedSound()->ClipCount;
			if (sameUnitClipFinished >= PreAnnoyedChatCount + annoyedCount)
				sameUnitClipFinished = 0;

			if (sameUnitClipFinished >= PreAnnoyedChatCount) {
				newChatRequest = { unit.Sounds.GetAnnoyedSound(), id };
				return;
			}
		}
	}

	newChatRequest = { unit.Sounds.GetWhatSound(), id };


}
void SoundSystem::PlayUnitCommand(EntityId id, const UnitDef& unit) {
	newChatRequest = { unit.Sounds.GetYesSound(), id };
}
void SoundSystem::PlayAdviserErrorMessage(const RaceDef& race, AdvisorErrorMessageType message)
{
	auto clip = race.AdvisorErrorSounds.Clips[(int)message];

	if (clip->GetData().Size() == 0)
		return;


	auto& channel = chatAudioChannel;

	if (clip->id == channel.clipId)
		return;

	channel.clipId = clip->id;

	AudioManager::PlayClip(clip, channel.channel->ChannelId);
}
void SoundSystem::PlayUISound(AudioClip& clip)
{
	AudioManager::PlayClip(&clip, uiAudioChannel.channel->ChannelId);
}

void SoundSystem::RegenerateSoundQueue(const SoundSetDef& def) {
	chatSoundQueue.clear();
	for (AudioClip* audio : def.GetAudioClips())
		chatSoundQueue.push_back(audio);

	if (def.Randomize) {

		for (int i = 0; i < chatSoundQueue.size() - 1; ++i) {
			int j = rnd.Next(i, chatSoundQueue.size());
			AudioClip* swap = chatSoundQueue[i];
			chatSoundQueue[i] = chatSoundQueue[j];
			chatSoundQueue[j] = swap;
		}
	}

}

void SoundSystem::UpdateChatRequest(EntityManager& em)
{
	bool newRequest = false;
	bool sameUnit = currentChat.id == newChatRequest.id;

	auto& channel = chatAudioChannel;


	if (newChatRequest.id != Entity::None &&
		(!sameUnit || currentChat.sound != newChatRequest.sound ||
			channel.channel->IsDone())) {

		if (currentChat.sound != newChatRequest.sound || !sameUnit) {
			chatSoundQueue.clear();
		}

		if (!sameUnit)
			sameUnitClipFinished = 0;

		currentChat = newChatRequest;
		newRequest = true;
	}

	newChatRequest = { nullptr, Entity::None, };


	if (newRequest && currentChat.sound) {

		AudioClip* play = nullptr;

		if (chatSoundQueue.size() == 0) {
			RegenerateSoundQueue(*currentChat.sound);
		}

		play = chatSoundQueue.front();
		chatSoundQueue.erase(chatSoundQueue.begin());

		AudioManager::PlayClip(play, channel.channel->ChannelId);
		channel.clipId = play->id;


	}


	if (currentChat.sound != nullptr) {

		if (channel.channel->IsDone() && channel.clipId != 0xFFFF) {

			++sameUnitClipFinished;
			channel.clipId = 0xFFFF;
		}
		else {
			if (!em.UnitArchetype.Archetype.HasEntity(currentChat.id)) {
				currentChat = { nullptr, Entity::None };
				AudioManager::StopChannel(channel.channel->ChannelId);
				sameUnitClipFinished = 0;
				channel.clipId = 0xFFFF;
			}
		}
	}

}

void SoundSystem::ClearAudio(EntityManager& em)
{
	for (auto& channel : worldAudioChannels) {
		AudioManager::StopChannel(channel.channel->ChannelId);
	}

	AudioManager::StopChannel(uiAudioChannel.channel->ChannelId);
	AudioManager::StopChannel(chatAudioChannel.channel->ChannelId);

	for (EntityId id : em.SoundArchetype.Archetype.GetEntities()) {

		auto& flags = em.FlagComponents.GetComponent(id);
		if (!flags.test(ComponentFlags::SoundTrigger))
			continue;

		flags.clear(ComponentFlags::SoundTrigger);
	}
}