#include "AudioManager.h"
#include "../Platform.h"
#include "../Profiler.h"

static constexpr const int AudioChannelBufferSize = 4096;
static constexpr const int MonoChannels = 6;

AudioManager AudioManager::instance;

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


void AudioManager::PlayClip(AudioClip* clip, int c)
{
	auto& channel = instance.channels[c];

	clip->Restart();
	clip->FillNextBuffer();

	channel.stream = clip;
	channel.ClearQueue();

	channel.QueueClip({ clip->GetData(),0 });

	Platform::EnableChannel(channel, true);
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

void AudioManager::UpdateAudio()
{
	for (auto& channel : instance.channels)
	{
		if (channel.IsValid())
		{
			auto* currentClip = channel.CurrentClip();

			if (channel.stream && !channel.stream->IsAtEnd())
			{
				AudioClip& track = *channel.stream;

				if (!channel.IsQueueFull())
				{
					track.FillNextBuffer();
					channel.QueueClip({ track.GetData(),0 });
				}
			}
			else
				if (currentClip == nullptr || currentClip->Done())
				{
					Platform::EnableChannel(channel, false);
				}
		}
	}
}