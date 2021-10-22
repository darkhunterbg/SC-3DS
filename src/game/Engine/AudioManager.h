#pragma once

#include "AudioChannel.h"

#include <array>
#include "Span.h"


#include "../MathLib.h"

class AudioManager {

private:
	std::vector<AudioChannelState> channels;

	AudioManager() {}

	AudioManager(const AudioManager&) = delete;
	AudioManager& operator=(const AudioManager&) = delete;


	static AudioManager instance;

public:

	static void Init();
	static void PlayBuffer(Span<uint8_t> buffer, int channel);
	static void Play(IAudioSource& source, int channel);
	static void StopChannel(int channel);
	static void SetChannelVolume(int channel, float volume);

	static void UpdateAudio();

	inline  static const Span<AudioChannelState> GetAudioChannes() {
		return { instance.channels.data(), instance.channels.size() };
	}
};