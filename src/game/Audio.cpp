#include "Audio.h"
#include "Platform.h"

static constexpr const int AudioChannelBufferSize = 4096;

AudioSystem::AudioSystem() {
	channel.bufferSize = AudioChannelBufferSize;
	Platform::CreateChannel(channel);
	Platform::EnableChannel(channel, false);
}

void AudioSystem::PlayClip(AudioClip clip) {
	channel.stream = nullptr;
	channel.ClearQueue();
	channel.QueueClip({ clip.GetData(), 0 });
	Platform::EnableChannel(channel, true);
}
void AudioSystem::PlayStream(AudioStream* stream)
{
	stream->Restart();
	stream->FillNextBuffer();

	channel.stream = stream;
	channel.ClearQueue();

	channel.QueueClip({ stream->GetData(),0 });

	Platform::EnableChannel(channel, true);

}

void AudioSystem::UpdateAudio() {

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