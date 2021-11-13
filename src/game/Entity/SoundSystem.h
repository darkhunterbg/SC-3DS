#pragma once

#include "IEntitySystem.h"
#include "../Random.h"
#include "../Assets.h"
#include "../Camera.h"
#include "../Data/SoundSetDef.h"
#include "../Engine/AudioChannel.h"
#include  <vector>

class EntityManager;

enum class UnitChatType {
	Ready = 0,
	Command = 1,
	Selected = 2,
	None = 255
};

class SoundSystem : public IEntitySystem {
public:
	struct Channel {
		const AudioChannelState* channel = nullptr;
		const SoundSetDef* sound = nullptr;
		float volume = 1.0f;
		int soundIndex = -1;
		bool newSound = false;
	};
	struct WorldSound {
		Vector2Int16 position;
		const SoundSetDef* sound;
		int priority = 0;
		float volume = 1;
	};
private:
	Random _rand;

	Channel _musicChannel;
	std::vector<Channel> _worldChannels;
	std::vector<WorldSound> _worldSounds;
	std::vector<WorldSound> _soundsInRange;
	Channel _chatChannel;

	EntityId _chatUnitId = Entity::None;
	UnitChatType _unitChatType = UnitChatType::None;
	int _unitChatCount = 0;

	void PlayDef(const SoundSetDef& def, Channel& channel, float volume = 1.0f);
	bool IsPlayCompleted(Channel& channel);

	std::vector<Channel*> _channels;
public:

	SoundSystem();

	void PlayMusic(const SoundSetDef& music);
	void PlayChat(const SoundSetDef& chat);
	void PlayUISound(const SoundSetDef* sound);
	bool PlayUnitChat(EntityId unit, UnitChatType type);

	void PlayWorldSound(const SoundSetDef& sound, Vector2Int16 pos, int priority);

	bool IsChatPlaying() { return !IsPlayCompleted(_chatChannel); }

	virtual void DeleteEntities(std::vector<EntityId>& entities) override;
	virtual size_t ReportMemoryUsage() override;

	void UpdateSounds(const EntityManager& em, const Camera& camera);

	const std::vector<Channel*>& GetChannels() const { return _channels; }
};