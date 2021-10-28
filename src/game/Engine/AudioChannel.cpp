#include "AudioChannel.h"

AudioChannelClip* AudioChannelState::CurrentClip()
{
	if (playingClip.IsDone() && streamingCrt != nullptr)
	{
		streamingCrt->RunAll();
		playingClip = streamingCrt->GetResult();

		if (!playingClip.IsEmpty())
			StartStreamingNextClip();
	}

	if (streamingCrt != nullptr)
		streamingCrt->Next();

	return playingClip.IsEmpty() ? nullptr : &playingClip;
}


bool AudioChannelState::IsStreaming() const
{
	return streamingCrt != nullptr;
}

void AudioChannelState::StartStreamingNextClip()
{
	if (stream != nullptr)
	{
		streamingCrt = stream->GetNextAudioChannelClipAsync();
		streamingCrt->Next();
	}
}

void AudioChannelState::StopStreaming()
{
	streamingCrt = nullptr;
}

bool AudioChannelState::IsDone() const
{
	return stream == nullptr || (playingClip.IsEmpty() && streamingCrt == nullptr);
}

void AudioChannelState::ChangeSource(IAudioSource* src)
{
	StopStreaming();
	stream = src;
	playingClip = {};
	StartStreamingNextClip();
}
