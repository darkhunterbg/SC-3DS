#pragma once

#include <vector>
#include "../Assets.h"
#include "Entity.h"


class EntityManager;
class Camera;
struct AudioChannelState;


class SoundSystem {
	typedef uint8_t* AudioClipId;
	typedef std::vector<Vector2Int16> WorldAudioClipQueue;

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

private:

	std::vector<AudioClip> queuedAudioClips;
	std::vector<WorldAudioClipQueue> queuedAudioPositions;
	std::vector<AudioClipId> queuedAudioIds;

	std::vector<const AudioChannelState*> audioChannels;

	std::vector<WorldAudioSetting> playWorldAudio;


	UnitChatRequest newChatRequest = {UnitChatType::None, Entity::None };
	UnitChatRequest currentChat = { UnitChatType::None, Entity::None };
public:
	SoundSystem();

	void UpdateEntityAudio(const Camera& camera);
	void UpdateChatRequest(EntityManager& em);
	void PlayWorldAudioClip(const AudioClip& clip, const Vector2Int16& worldPosition);
	void PlayWorldAudioClips(const std::vector< AudioClip>& clips, const std::vector < Vector2Int16>& positions);
	void PlayUnitChatSelect(EntityId id);
	void PlayUnitChatCommand(EntityId id);
};