#pragma once

#include "Audio.h"


struct NDSAudioChannel {
	bool enabled;
	AudioChannelState* state = nullptr;
	ndspWaveBuf waveBuff[2];
	ndspWaveBuf* freeBuff = nullptr;
	ndspWaveBuf* usedBuff = nullptr;
	NDSAudioChannel() {
		freeBuff = &waveBuff[0];
		usedBuff = &waveBuff[1];
	}
	void SwapBuffers() {
		auto temp = freeBuff;
		freeBuff = usedBuff;
		usedBuff = temp;
	}
};