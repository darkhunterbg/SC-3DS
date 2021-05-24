#pragma once

#include <vector>
#include "../Assets.h"
#include "Entity.h"


class EntityManager;
class Camera;
struct AudioChannelState;


class SoundSystem {
	typedef uint8_t* AudioClipId;

	enum UnitChatType {
		None = 0,
		Select = 1,
		Command = 2
	};

	struct UnitChatRequest {
		UnitChatType type;
		EntityId id;
	};

	struct WorldAudioSetting {
		AudioClip clip;
		float volume = 1.0f;
	};

	struct EntityAudio {
		uint16_t priority = 0;
		uint16_t clipIndex = 0;
	};

private:
	std::vector<const AudioChannelState*> audioChannels;

	std::vector<AudioClip> entityUniqueAudio;
	std::vector<EntityAudio> entityAudioPriority;

	std::vector<WorldAudioSetting> playWorldAudio;

	UnitChatRequest newChatRequest = { UnitChatType::None, Entity::None };
	UnitChatRequest currentChat = { UnitChatType::None, Entity::None };

	void CollectAudioFromSources(const Camera& camera, EntityManager& em);

	static bool EntityAudioSort(const EntityAudio& a, EntityAudio& b);
public:
	SoundSystem();

	void UpdateEntityAudio(const Camera& camera, EntityManager& em);
	void UpdateChatRequest(EntityManager& em);
	void PlayUnitChatSelect(EntityId id);
	void PlayUnitChatCommand(EntityId id);
};