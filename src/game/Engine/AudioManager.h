#pragma once

#include "AudioChannel.h"

#include <array>
#include "Span.h"


#include "../MathLib.h"

class AudioManager {

private:
	std::vector<AudioChannelState> channels;
	bool _mute = false;

	AudioManager() {}

	AudioManager(const AudioManager&) = delete;
	AudioManager& operator=(const AudioManager&) = delete;


	static AudioManager instance;

public:
	static bool IsMute()
	{
		return instance._mute;
	}
	static void SetMute(bool mute);

	static void Init();
	static void Play(IAudioSource& source, int channel);
	static void StopChannel(int channel);
	static void SetChannelVolume(int channel, float volume);
	static void StopAll();
	static void UpdateAudio();

	inline  static const Span<AudioChannelState> GetAudioChannels() {
		return { instance.channels.data(), instance.channels.size() };
	}
};