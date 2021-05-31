#pragma once

#include "AudioChannel.h"

#include <array>
#include "Span.h"


class AudioManager {

private:
	std::vector<AudioChannelState> channels;

	AudioManager() {}

	AudioManager(const AudioManager&) = delete;
	AudioManager& operator=(const AudioManager&) = delete;


	static AudioManager instance;

public:

	static void Init();
	static void PlayClip(const AudioClip&, int channel);
	static void PlayStream(AudioStream* stream, int channel);
	static void StopChannel(int channel);
	static void SetChannelVolume(int channel, float volume);

	static void UpdateAudio();

	inline  static const Span<AudioChannelState> GetAudioChannes() {
		return { instance.channels.data(), instance.channels.size() };
	}
};