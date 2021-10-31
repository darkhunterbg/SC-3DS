#include "AudioManager.h"
#include "../Platform.h"
#include "../Profiler.h"
#include "AssetLoader.h"
#include "../Coroutine.h"
#include "../Debug.h"

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
		channel.SetEnabled(!mute);
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
		instance.channels[i].SetEnabled(false);

		unsigned size = instance.channels[i].bufferSize;
		uint8_t* mem = (uint8_t*)Platform::PlatformAlloc(size * 2);
		instance.channels[i].InitBuffer({ mem , size }, { mem + size, size });
	}
}



void AudioManager::Play(IAudioSource& src, int c)
{
	GAME_ASSERT(c < instance.channels.size(), "Tried to play clip on invalid channel %i", c);

	auto& channel = instance.channels[c];


	Platform::EnableChannel(channel, !instance._mute);
	channel.SetEnabled(!instance._mute);

	channel.ChangeSource(&src, 0);
}


void AudioManager::StopChannel(int c)
{
	auto& channel = instance.channels[c];
	channel.ChangeSource(nullptr);
	Platform::EnableChannel(channel, false);
	channel.SetEnabled(false);
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
		channel.ChangeSource(nullptr);
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
		{
			if (channel.GetEnabled())
			{
				Platform::EnableChannel(channel, false);
				channel.SetEnabled(false);
			}
		}
		else
		{
			if (!channel.GetEnabled())
			{
				Platform::EnableChannel(channel, true);
				channel.SetEnabled(true);
			}

		}
	}
}