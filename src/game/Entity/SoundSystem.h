#pragma once

#include "IEntitySystem.h"
#include "../Random.h"
#include "../Assets.h"
#include "../Data/SoundSetDef.h"
#include "../Engine/AudioChannel.h"
#include  <vector>

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

	void PlayDef(const SoundSetDef& def, Channel& channel);
public:

	SoundSystem();

	void PlayMusic(const SoundSetDef& music);
	void PlayChat(const SoundSetDef& chat);

	virtual void DeleteEntities(std::vector<EntityId>& entities) override;
	virtual size_t ReportMemoryUsage() override;
};