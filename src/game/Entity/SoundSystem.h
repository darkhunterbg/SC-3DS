#pragma once

#include <vector>
#include "../Assets.h"
#include "Entity.h"
#include "Common.h"
#include "../Random.h"

class EntityManager;
class Camera;
struct AudioChannelState;
struct RaceDef;
struct SoundSetDef;
struct UnitDef;

class SoundSystem {
	
	struct UnitChatRequest {
		const SoundSetDef* sound;
		EntityId id;
	};

	struct EntityAudio {
		AudioClip* clip = nullptr;
		float volume = 1.0f;
		uint16_t priority = 0;
	};

	struct EntityPriorityAudio {
		uint16_t priority = 0;
		uint16_t clipIndex = 0;
		float volume = 0;
	};

	struct EntityAudioChannel {
		const AudioChannelState* channel = nullptr;
		uint16_t clipId;
		uint16_t clipPriority;
		bool queued = false;
	};

private:
	std::vector<EntityAudioChannel> worldAudioChannels;
	EntityAudioChannel chatAudioChannel;
	EntityAudioChannel uiAudioChannel;

	std::vector<AudioClip*> entityUniqueAudio;
	std::vector<EntityPriorityAudio> entityAudioPriority;

	std::vector<EntityAudio> playWorldAudio;

	UnitChatRequest newChatRequest = { nullptr, Entity::None };
	UnitChatRequest currentChat = { nullptr, Entity::None };


	std::vector<AudioClip*> chatSoundQueue;
	int sameUnitClipFinished = 0;

	Random rnd;

	void CollectAudioFromSources(const Camera& camera, EntityManager& em);
	void RegenerateSoundQueue(const SoundSetDef& def);

	static bool EntityAudioSort(const EntityPriorityAudio& a, EntityPriorityAudio& b);

public:
	SoundSystem();

	void UpdateEntityAudio(const Camera& camera, EntityManager& em);
	void UpdateChatRequest(EntityManager& em);
	void PlayUnitReady(EntityId id, const UnitDef& unit);
	void PlayUnitSelect(EntityId id, const UnitDef& unit);
	void PlayUnitCommand(EntityId id, const UnitDef& unit);
	void PlayAdviserErrorMessage(const RaceDef& race, AdvisorErrorMessageType message);
	void PlayUISound(AudioClip& clip);

	void ClearAudio(EntityManager& em);
};