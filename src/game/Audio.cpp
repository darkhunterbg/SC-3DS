#include "Audio.h"
#include "Platform.h"

static constexpr const int AudioChannelBufferSize = 4096 ;

void AudioSystem::Init() {
	channels[1].mono = true;

	for (int i = 0; i < channels.size(); i++) {
		channels[i].bufferSize = AudioChannelBufferSize / (channels[i].mono ? 2 : 1);
		Platform::CreateChannel(channels[i]);
		Platform::EnableChannel(channels[i], false);
	}
}

void AudioSystem::PlayClip(AudioClip clip, int c) {
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