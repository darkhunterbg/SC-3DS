#pragma once

#include <vector>
#include "../Assets.h"
#include "Entity.h"
#include "Common.h"

class EntityManager;
class Camera;
struct AudioChannelState;


class SoundSystem {
	
	struct UnitChatRequest {
		UnitChatType type;
		EntityId id;
	};

	struct EntityAudio {
		AudioClip clip;
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

	std::vector<AudioClip> entityUniqueAudio;
	std::vector<EntityPriorityAudio> entityAudioPriority;

	std::vector<EntityAudio> playWorldAudio;

	unsigned long seed = 0;

	UnitChatRequest newChatRequest = { UnitChatType::None, Entity::None };
	UnitChatRequest currentChat = { UnitChatType::None, Entity::None };

	void CollectAudioFromSources(const Camera& camera, EntityManager& em);

	static bool EntityAudioSort(const EntityPriorityAudio& a, EntityPriorityAudio& b);
public:
	SoundSystem();

	void UpdateEntityAudio(const Camera& camera, EntityManager& em);
	void UpdateChatRequest(EntityManager& em);
	void PlayUnitChat(EntityId id, UnitChatType type);

	void PlayUISound(const AudioClip& clip);
};