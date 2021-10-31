#pragma once

#include "IEntitySystem.h"
#include "../Random.h"
#include "../Assets.h"
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
	struct Channel {
		const AudioChannelState* channel = nullptr;
		const SoundSetDef* sound = nullptr;
		float volume = 1.0f;
		int soundIndex = -1;
	};
private:
	Random _rand;

	Channel _musicChannel;
	std::vector<Channel> _worldChannels;
	Channel _chatChannel;

	EntityId _chatUnitId = Entity::None;
	UnitChatType _unitChatType = UnitChatType::None;
	int _unitChatCount = 0;

	void PlayDef(const SoundSetDef& def, Channel& channel);
	bool IsPlayCompleted(Channel& channel);
public:

	SoundSystem();

	void PlayMusic(const SoundSetDef& music);
	void PlayChat(const SoundSetDef& chat);
	bool PlayUnitChat(EntityId unit, UnitChatType type);

	bool IsChatPlaying() { return !IsPlayCompleted(_chatChannel); }

	virtual void DeleteEntities(std::vector<EntityId>& entities) override;
	virtual size_t ReportMemoryUsage() override;

	void UpdateSounds(const EntityManager& em);
};