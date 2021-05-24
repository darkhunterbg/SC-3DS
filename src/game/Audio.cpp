#include "Audio.h"
#include "Platform.h"

static constexpr const int AudioChannelBufferSize = 4096 ;
static constexpr const int MonoChannels = 5;

void AudioSystem::Init() {

	AudioChannelState channel;
	channel.mono = false;

	channels.push_back(channel);
	
	channel.mono = true;
	for (int i = 0; i < MonoChannels; ++i) {
		channels.push_back(channel);
	}

	for (int i = 0; i < channels.size(); i++) {
		channels[i].bufferSize = AudioChannelBufferSize / (channels[i].mono ? 2 : 1);
		channels[i].ChannelId = i;
		Platform::CreateChannel(channels[i]);
		Platform::EnableChannel(channels[i], false);
	}
}

void AudioSystem::PlayClip(const AudioClip& clip, int c) {
	auto& channel = channels[c];
	channel.stream = nullptr;
	channel.ClearQueue();
	channel.QueueClip({ clip.GetData(), 0 });
	Platform::EnableChannel(channel, true);
}
void AudioSystem::PlayStream(AudioStream* stream, int c)
{
	auto& channel = channels[c];

	stream->Restart();
	stream->FillNextBuffer();

	channel.stream = stream;
	channel.ClearQueue();

	channel.QueueClip({ stream->GetData(),0 });

	Platform::EnableChannel(channel, true);
}

void AudioSystem::StopChannel(int c)
{
	auto& channel = channels[c];
	Platform::EnableChannel(channel, false);
}

void AudioSystem::SetChannelVolume(int c, float volume)
{
	channels[c].volume = std::min(1.0f, std::max(0.0f, volume));
}

void AudioSystem::UpdateAudio() {

	for (auto& channel : channels) {

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