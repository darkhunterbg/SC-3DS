#include "AudioManager.h"
#include "Platform.h"

static constexpr const int AudioChannelBufferSize = 4096;
static constexpr const int MonoChannels = 6;

AudioManager AudioManager::instance;

void AudioManager::Init() {

	AudioChannelState channel;
	channel.mono = false;

	instance.channels.push_back(channel);

	channel.mono = true;
	for (int i = 0; i < MonoChannels; ++i) {
		instance.channels.push_back(channel);
	}

	for (int i = 0; i < instance.channels.size(); i++) {
		instance.channels[i].bufferSize = AudioChannelBufferSize / (instance.channels[i].mono ? 2 : 1);
		instance.channels[i].ChannelId = i;
		Platform::CreateChannel(instance.channels[i]);
		Platform::EnableChannel(instance.channels[i], false);
	}
}

void AudioManager::PlayClip(const AudioClip& clip, int c) {
	auto& channel = instance.channels[c];
	channel.stream = nullptr;
	channel.ClearQueue();
	channel.QueueClip({ clip.GetData(), 0 });
	Platform::EnableChannel(channel, true);
}
void AudioManager::PlayStream(AudioStream* stream, int c)
{
	auto& channel = instance.channels[c];

	stream->Restart();
	stream->FillNextBuffer();

	channel.stream = stream;
	channel.ClearQueue();

	channel.QueueClip({ stream->GetData(),0 });

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

void AudioManager::UpdateAudio() {

	for (auto& channel : instance.channels) {

		if (channel.IsValid()) {

			auto* currentClip = channel.CurrentClip();

			if (channel.stream && !channel.stream->IsAtEnd()) {
				AudioStream& track = *channel.stream;

				if (!channel.IsQueueFull()) {
					track.FillNextBuffer();
					channel.QueueClip({ track.GetData(),0 });
				}
			}
			else
				if (currentClip == nullptr || currentClip->Done()) {
					Platform::EnableChannel(channel, false);
				}
		}
	}
}