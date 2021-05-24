#include "SoundSystem.h"
#include "../Game.h"
#include "../Camera.h"
#include "EntityManager.h"
#include "../Profiler.h"

#include <algorithm>

SoundSystem::SoundSystem()
{
	for (const auto& channel : Game::Audio.GetAudioChannes()) {
		if (channel.mono)
			audioChannels.push_back(&channel);
	}
}

bool SoundSystem::EntityAudioSort(const EntityAudio& a, EntityAudio& b) {
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

		const SoundSourceComponent& src = em.SoundArchetype.SourceComponents.GetComponent(id);
		const Vector2Int16& pos = em.PositionComponents.GetComponent(id);

		if (extendedCamRect.Contains(pos)) {

			bool isFullAudio = camRect.Contains(pos);

			int foundIndex = -1;

			int i = 0;
			for (const auto& clip : entityUniqueAudio) {
				if (clip.data == src.clip.data)
				{
					foundIndex = i;
					break;
				}
				++i;
			}

			if (foundIndex < 0) {
				entityUniqueAudio.push_back(src.clip);
				uint16_t priority = isFullAudio;
				entityAudioPriority.push_back({ priority, (uint16_t)entityAudioPriority.size() });
			}
			else if (isFullAudio) {
				entityAudioPriority[foundIndex].priority = isFullAudio;
			}
		}
	}

	std::sort(entityAudioPriority.begin(), entityAudioPriority.end(), EntityAudioSort);

	int max = std::min(entityUniqueAudio.size(), audioChannels.size() - 1);

	for (int i = 0; i < max; ++i) {
		int index = entityAudioPriority[i].clipIndex;
		const AudioClip& clip = entityUniqueAudio[index];
		float volume = entityAudioPriority[i].priority > 0 ? 1 : 0.5f;
		playWorldAudio.push_back({ clip,volume });
	}

}

void SoundSystem::UpdateEntityAudio(const Camera& camera, EntityManager& em)
{
	SectionProfiler p("EntityAudio");

	CollectAudioFromSources(camera, em);

	int max = std::min(playWorldAudio.size(), audioChannels.size() - 1);

	for (int i = 0; i < max; ++i) {
		auto channel = audioChannels[i];

		for (int j = 0; j < audioChannels.size() - 1; ++j) {
			auto channel = audioChannels[j];
			if (!channel->IsDone())
				continue;

			auto& clip = playWorldAudio[i].clip;

			Game::Audio.SetChannelVolume(channel->ChannelId, playWorldAudio[i].volume);
			Game::Audio.PlayClip(clip, channel->ChannelId);

			break;
		}

	}
}


void SoundSystem::PlayUnitChatSelect(EntityId id)
{
	newChatRequest = { UnitChatType::Select, id };
}

void SoundSystem::PlayUnitChatCommand(EntityId id)
{
	newChatRequest = { UnitChatType::Command, id };
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
				Game::Audio.PlayClip(sound->Clips[i], channel->ChannelId);
			}
		}

	}

	if (currentChat.id != Entity::None && channel->IsDone()) {
		currentChat = { UnitChatType::None, Entity::None };
	}
}

