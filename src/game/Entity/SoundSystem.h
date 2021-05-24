#pragma once

#include <vector>
#include "../Assets.h"
#include "Entity.h"


class EntityManager;
class Camera;
struct AudioChannelState;


class SoundSystem {
	enum UnitChatType {
		None = 0,
		Select = 1,
		Command = 2
	};

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
	};

	struct EntityAudioChannel {
		const AudioChannelState* channel = nullptr;
		uint16_t clipId;
		uint16_t clipPriority;
		bool queued = false;
	};

private:
	std::vector<EntityAudioChannel> audioChannels;

	std::vector<AudioClip> entityUniqueAudio;
	std::vector<EntityPriorityAudio> entityAudioPriority;

	std::vector<EntityAudio> playWorldAudio;

	UnitChatRequest newChatRequest = { UnitChatType::None, Entity::None };
	UnitChatRequest currentChat = { UnitChatType::None, Entity::None };

	void CollectAudioFromSources(const Camera& camera, EntityManager& em);

	static bool EntityAudioSort(const EntityPriorityAudio& a, EntityPriorityAudio& b);
public:
	SoundSystem();

	void UpdateEntityAudio(const Camera& camera, EntityManager& em);
	void UpdateChatRequest(EntityManager& em);
	void PlayUnitChatSelect(EntityId id);
	void PlayUnitChatCommand(EntityId id);
};