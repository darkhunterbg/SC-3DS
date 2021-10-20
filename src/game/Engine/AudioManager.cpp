#include "AudioManager.h"
#include "../Platform.h"
#include "../Profiler.h"
#include "AssetLoader.h"
#include "../Coroutine.h"

static constexpr const int AudioChannelBufferSize = 4096;
static constexpr const int MonoChannels = 6;

AudioManager AudioManager::instance;

static void FinishBufferingAndPlay(AudioChannelState& channel)
{
	GAME_ASSERT(channel.IsStreaming(), "Channel %i is not streaming!");
	channel.streamingCrt->RunAll();

	channel.stream->SwapBuffers();
	channel.QueueClip({ channel.stream->GetData(),0 });

	delete channel.streamingCrt;
	channel.streamingCrt = nullptr;

	Platform::EnableChannel(channel, true);
}

static void StartLoadingNextBuffer(AudioChannelState& channel)
{
	GAME_ASSERT(channel.stream != nullptr, "Channel %i does not have audo stream to buffer from!", channel.ChannelId);
	GAME_ASSERT(!channel.IsStreaming(), "Channel %i is already buffering!");

	channel.streamingCrt = AssetLoader::RunIOAsync([channel]() {
		channel.stream->FillNextBuffer();
		});
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


void AudioManager::PlayClip(AudioClip* clip, int c)
{
	auto& channel = instance.channels[c];

	clip->Restart();

	if (channel.IsStreaming())
	{
		delete channel.streamingCrt;
		channel.streamingCrt = nullptr;
	}

	channel.stream = clip;
	channel.ClearQueue();

	

	StartLoadingNextBuffer(channel);
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
		if (!channel.IsValid())
			continue;


		bool isStreamDone = !channel.IsStreaming() && (channel.stream == nullptr || channel.stream->IsAtEnd());
		bool isPlayingDone = channel.CurrentClip() == nullptr || channel.CurrentClip()->Done();

		if (isPlayingDone && isStreamDone)
		{
			Platform::EnableChannel(channel, false);
			continue;
		}


		if (channel.stream == nullptr)
			continue;


		if (!channel.stream->IsAtEnd())
		{
			AudioClip& track = *channel.stream;

			if (!channel.IsQueueFull())
			{
				if (!channel.IsStreaming())
					StartLoadingNextBuffer(channel);
			}

			if (channel.IsDone() && channel.IsStreaming())
			{
				FinishBufferingAndPlay(channel);
			}
		}
	}
}