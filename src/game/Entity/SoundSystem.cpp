#include "SoundSystem.h"
#include "../Game.h"
#include "../Camera.h"
#include "EntityManager.h"
#include "../Profiler.h"

#include "../MathLib.h"
#include "../Platform.h"

#include <algorithm>



SoundSystem::SoundSystem()
{
	for (const auto& channel : Game::Audio.GetAudioChannes()) {
		if (channel.mono) {
			EntityAudioChannel eac;
			eac.channel = &channel;
			audioChannels.push_back(eac);
		}
	}

	seed = Platform::ElaspedTime();
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
				if (clip.id == src.clip->id)
				{
					foundIndex = i;
					break;
				}
				++i;
			}

			if (foundIndex < 0) {
				entityUniqueAudio.push_back(*src.clip);
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

	int max = std::min(entityUniqueAudio.size(), audioChannels.size() - 1);

	for (int i = 0; i < max; ++i) {
		int index = entityAudioPriority[i].clipIndex;
		const AudioClip& clip = entityUniqueAudio[index];
		uint16_t priority = entityAudioPriority[i].priority;
		float volume = entityAudioPriority[i].volume;

		playWorldAudio.push_back({ clip,volume, priority });
	}

}

void SoundSystem::UpdateEntityAudio(const Camera& camera, EntityManager& em)
{
	CollectAudioFromSources(camera, em);

	int max = std::min(playWorldAudio.size(), audioChannels.size() - 1);

	for (int j = 0; j < audioChannels.size() - 1; ++j) {
		audioChannels[j].queued = false;
	}

	for (int i = 0; i < max; ++i) {

		auto& clip = playWorldAudio[i].clip;

		bool channelFound = false;

		for (int j = 0; j < audioChannels.size() - 1; ++j) {
			auto& channel = audioChannels[j];
			if (channel.queued)
				continue;

			if (channel.channel->IsDone() || channel.clipId == clip.id)
			{
				Game::Audio.SetChannelVolume(channel.channel->ChannelId, playWorldAudio[i].volume);
				Game::Audio.PlayClip(clip, channel.channel->ChannelId);
				channel.clipId = clip.id;
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

			for (int j = 0; j < audioChannels.size() - 1; ++j) {
				auto& channel = audioChannels[j];
				if (channel.queued)
					continue;

				if (channel.clipPriority < prio) {
					channelId = j;
					prio = channel.clipPriority;
				}

			}

			if (channelId != -1) {
				channelFound = true;
				auto& channel = audioChannels[channelId];

				Game::Audio.SetChannelVolume(channel.channel->ChannelId, playWorldAudio[i].volume);
				Game::Audio.PlayClip(clip, channel.channel->ChannelId);
				channel.clipId = clip.id;
				channel.queued = true;
				channel.clipPriority = playWorldAudio[i].priority;
				channelFound = true;
			}
		}
	}


}


void SoundSystem::PlayUnitChat(EntityId id, UnitChatType type)
{
	newChatRequest = { type, id };
}


void SoundSystem::UpdateChatRequest(EntityManager& em)
{
	bool newRequest = false;

	if (newChatRequest.id != Entity::None &&
		(currentChat.id != newChatRequest.id || currentChat.type != newChatRequest.type)) {

		currentChat = newChatRequest;
		newRequest = true;
	}

	newChatRequest = { UnitChatType::None, Entity::None, };

	auto channel = audioChannels.back();

	if (newRequest) {
		if (em.UnitArchetype.Archetype.HasEntity(currentChat.id)) {
			const UnitDef* def = em.UnitArchetype.UnitComponents.GetComponent(currentChat.id).def;
			const UnitSound* sound = nullptr;

			std::srand(seed);

			if (currentChat.type == UnitChatType::Command) {
				if (def->Sounds.Yes.TotalClips) {
					sound = &def->Sounds.Yes;

				}
			}
			else {
				if (def->Sounds.What.TotalClips) {
					sound = &def->Sounds.What;
				}
			}

			if (sound) {
				int i = std::rand() % sound->TotalClips;
				Game::Audio.PlayClip(sound->Clips[i], channel.channel->ChannelId);
			}
		}

	}

	if (currentChat.id != Entity::None) {

		if (channel.channel->IsDone()) {
			currentChat = { UnitChatType::None, Entity::None };
		}
		else {
			if (!em.UnitArchetype.Archetype.HasEntity(currentChat.id)) {
				currentChat = { UnitChatType::None, Entity::None };
				Game::Audio.StopChannel(channel.channel->ChannelId);
			}
		}
	}

	if (currentChat.id != Entity::None && channel.channel->IsDone()) {

	}
}

