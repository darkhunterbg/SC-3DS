#pragma once

#include "Engine/AudioChannel.h"


struct NDSAudioChannel {
	bool enabled;
	uint8_t id = 0;
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