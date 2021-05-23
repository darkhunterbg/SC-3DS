#include "SoundSystem.h"
#include "../Game.h"
#include "../Camera.h"
#include "EntityManager.h"
#include "../Profiler.h"

SoundSystem::SoundSystem()
{
	for (const auto& channel : Game::Audio.GetAudioChannes()) {
		if (channel.mono)
			audioChannels.push_back(&channel);
	}
}

void SoundSystem::UpdateEntityAudio(const Camera& camera)
{
	SectionProfiler p("EntityAudio");
	

	Rectangle16 camRect = camera.GetRectangle16();
	Rectangle16 extendedCamRect = camRect;
	extendedCamRect.size = (extendedCamRect.size * 3) / 2;
	extendedCamRect.SetCenter(camRect.GetCenter());

	playWorldAudio.clear();

	for (int i = 0; i < queuedAudioClips.size(); ++i) {
		auto& queue = queuedAudioPositions[i];



		if (queue.size() == 0)
			continue;

		float volume = 0.0f;

		for (const Vector2Int16& pos : queue) {
			if (extendedCamRect.Contains(pos)) {
				volume = 0.5f;

				if (camRect.Contains(pos)) {
					volume = 1.0f;
					break;
				}
			}
		}

		if (volume > 0) {
			playWorldAudio.push_back({ queuedAudioClips[i], volume });
		}

		queue.clear();
	}

	int max = std::min(playWorldAudio.size(), audioChannels.size() - 1);

	// TODO: Priority for sounds 

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


void SoundSystem::PlayWorldAudioClip(const AudioClip& clip, const Vector2Int16& worldPosition)
{
	AudioClipId id = clip.data;

	for (int i = 0; i < queuedAudioIds.size(); ++i) {
		if (queuedAudioIds[i] == id) {
			queuedAudioPositions[i].push_back(worldPosition);
			return;
		}
	}

	queuedAudioIds.push_back(id);
	queuedAudioClips.push_back(clip);
	queuedAudioPositions.push_back(WorldAudioClipQueue());
	queuedAudioPositions.back().push_back(worldPosition);
}

void SoundSystem::PlayWorldAudioClips(const std::vector<AudioClip>& clips, const std::vector<Vector2Int16>& positions)
{
	for (int j = 0; j < clips.size(); ++j) {
		const AudioClip& clip = clips[j];
		const Vector2Int16& pos = positions[j];

		PlayWorldAudioClip(clip, pos);
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

