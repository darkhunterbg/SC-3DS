#include "AudioManager.h"
#include "../Platform.h"
#include "../Profiler.h"
#include "AssetLoader.h"
#include "../Coroutine.h"

static constexpr const int AudioChannelBufferSize = 4096;
static constexpr const int MonoChannels = 6;

AudioManager AudioManager::instance;

void AudioManager::SetMute(bool mute)
{
	instance._mute = mute;


	for (auto& channel : instance.channels)
	{
		if (!channel.IsValid())
			continue;

		Platform::EnableChannel(channel, !mute);
	}
}

void AudioManager::Init()
{
	AudioChannelState channel;
	channel.mono = false;

	instance.channels.push_back(channel);

	channel.mono = true;
	for (int i = 0; i < MonoChannels; ++i)
	{
		instance.channels.push_back(channel);
	}

	for (int i = 0; i < instance.channels.size(); i++)
	{
		instance.channels[i].bufferSize = AudioChannelBufferSize / (instance.channels[i].mono ? 2 : 1);
		instance.channels[i].ChannelId = i;
		Platform::CreateChannel(instance.channels[i]);
		Platform::EnableChannel(instance.channels[i], false);
	}
}



void AudioManager::Play(IAudioSource& src, int c)
{
	GAME_ASSERT(c < instance.channels.size(), "Tried to play clip on  invalid channel %i", c);

	auto& channel = instance.channels[c];

	src.Restart();
	channel.ChangeSource(&src);

	Platform::EnableChannel(channel, !instance._mute);
}

void AudioManager::StopChannel(int c)
{
	auto& channel = instance.channels[c];
	Platform::EnableChannel(channel, false);
}

void AudioManager::SetChannelVolume(int c, float volume)
{
	instance.channels[c].volume = std::min(1.0f, std::max(0.0f, volume));
}

void AudioManager::StopAll()
{
	for (auto& channel : instance.channels)
	{
		if (!channel.IsValid())
			continue;

		Platform::EnableChannel(channel, false);
	}
}

void AudioManager::UpdateAudio()
{
	if (instance._mute)
		return;

	for (auto& channel : instance.channels)
	{
		if (!channel.IsValid())
			continue;

		if (channel.IsDone())
			Platform::EnableChannel(channel, false);
		else
			Platform::EnableChannel(channel, true);
	}
}