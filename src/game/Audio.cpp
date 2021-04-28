#include "Audio.h"
#include "Platform.h"

static constexpr const int AudioChannelBufferSize = 4096;

AudioSystem::AudioSystem() {
	channel.bufferSize = AudioChannelBufferSize;
	Platform::CreateChannel(channel);
	Platform::EnableChannel(channel, false);
}

void AudioSystem::PlayClip(AudioClip clip) {
	//channel.track = nullptr;
	channel.ClearQueue();
	channel.QueueClip({ clip.GetData(), 0 });
	Platform::EnableChannel(channel, true);
}

void AudioSystem::UpdateAudio() {

	//AudioChannelState& channel = channel;

	if (channel.IsValid()) {

		auto* currentClip = channel.CurrentClip();

		//if (channel.track && !channel.track->IsAtEnd()) {
		//	AudioTrack& track = *channel.track;

		//	if (!channel.clipQueue.Full()) {
		//		track.FillNextBuffer();
		//		channel.QueueClip({ track.GetData(),0 });
		//	}
		//}
		//else
		  if (currentClip == nullptr || currentClip->Done()) {
			Platform::EnableChannel(channel, false);
		}
	}
}