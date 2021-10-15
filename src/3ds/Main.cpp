#include <3ds.h>
#include <citro3d.h>
#include <citro2d.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <string>
#include <iostream>

#include "Color.h"
#include "Game.h"
#include "StringLib.h"

#include "NDSAudioChannel.h"

#include "Profiler.h"

#include "Engine/GraphicsPrimitives.h"
#include "Engine/AudioChannel.h"
#include "RenderTarget.h"

#include <spritebatch_shbin.h>

C3D_RenderTarget* top;
C3D_RenderTarget* bottom;
RenderTarget screens[2];
std::string assetDir;
std::string userDir;
C2D_TextBuf textBuffer;
u64 mainTimer;
std::vector<NDSAudioChannel*> audioChannels;

void Init();
void Uninit();
void UpdateAudioChannel(NDSAudioChannel& channel);

PrintConsole* console = NULL;

void FatalError(const char* error, ...)
{
	if (console == nullptr)
		console = consoleInit(GFX_BOTTOM, nullptr);

	int i = 0;
	for (auto channel : audioChannels)
	{
		ndspChnSetPaused(i++, true);
	}

	va_list args;
	va_start(args, error);
	char buffer[1024];
	stbsp_vsnprintf(buffer, 1024, error, args);
	va_end(args);

	std::cout << CONSOLE_RED << buffer << CONSOLE_RESET << std::endl;
	std::cout << "Press START to exit";

	while (aptMainLoop())
	{
		hidScanInput();
		if (hidKeysDown() & KEY_START)
			break;

		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		C3D_FrameEnd(0);
	}

	Uninit();
	exit(1);
}

struct Atlas {
	const char* name;
	int subatlases = 0;
};

u32 __ctru_heap_size = (30 << 20);


int main()
{
	//TextureLoadTest();

	Init();

	mainTimer = svcGetSystemTick();

	Game::Start();

	while (aptMainLoop())
	{
		svcSleepThread(1);

		hidScanInput();

		u32 kDown = hidKeysDown();
		if (kDown & KEY_START)
			break;


		for (auto channel : audioChannels)
		{
			if (!channel->enabled)
				continue;

			UpdateAudioChannel(*channel);
		}


		bool done = !Game::Update();

		if (done)
			break;

		Game::FrameEnd();

		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

		Game::FrameStart();

		u32 color = C2D_Color32f(Colors::CornflowerBlue.r, Colors::CornflowerBlue.g, Colors::CornflowerBlue.b, Colors::CornflowerBlue.a);
		C2D_TargetClear(top, color);
		C2D_TargetClear(bottom, color);
		Game::Draw();

		C3D_FrameEnd(0);
	}

	Game::End();

	Uninit();

	return 0;
}


void Init()
{

	romfsInit();
	gfxInitDefault();
	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);

	// Enable N3DS 804MHz operation, where available
	//osSetSpeedupEnable(true);

	//Result r = APT_SetAppCpuTimeLimit(30);
	//if (r)
	//	FatalError("svcReleaseSemaphore failed with %s", R_SUMMARY(r));


	C2D_Init(4096 * 8);
	C2D_Prepare();
	top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
	bottom = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);
	screens[0] = { top, nullptr, {400,240} };
	screens[1] = { bottom, nullptr, {320,240} };
	screens[0].Init();
	screens[1].Init();


	ndspInit();
	ndspSetOutputMode(NDSP_OUTPUT_STEREO);

	char dir[64];
	getcwd(&dir[0], 64);

	if (strcmp(dir, "/") == 0)
		assetDir = "3ds/StarCraft/romfs_debug/";
	else
		assetDir = "StarCraft/romfs_debug/";


	userDir = assetDir.substr(0, assetDir.length() - 6) + "User/";
	struct stat s = { 0 };

	if (stat(userDir.data(), &s) != 0 || !S_ISDIR(s.st_mode))
	{
		mkdir(userDir.data(), 0444);
	}

	textBuffer = C2D_TextBufNew(4096);
}
void Uninit()
{

	ndspExit();
	C2D_Fini();
	C3D_Fini();
	gfxExit();
	romfsExit();
}

void UpdateAudioChannel(NDSAudioChannel& channel)
{

	// TODO: on update check if clip has changed and if so, clear chain and start over 

	auto state = channel.state;

	ndspWaveBuf& usedBuff = *channel.freeBuff;
	ndspWaveBuf& freeBuff = *channel.freeBuff;

	// Wait for playing buffer to finish
	if (usedBuff.status == NDSP_WBUF_QUEUED || usedBuff.status == NDSP_WBUF_PLAYING)
		return;

	AudioChannelClip* clip = state->CurrentClip();
	unsigned size = clip != nullptr ? clip->Remaining() : 0;
	if (size == 0)
	{
		//EngineLogWarning("Voice starvation at channel %i", state->handle);
		return;
	}

	float volume[12];
	for (int i = 0; i < 12; ++i)
		volume[i] = state->volume * 0.8f;
	ndspChnSetMix(state->handle, volume);

	auto _audioBuffer = (u8*)freeBuff.data_vaddr;
	unsigned len = state->bufferSize;
	len = (len > size ? size : len);

	memcpy(_audioBuffer, clip->PlayFrom(), len);

	if (len < state->bufferSize)
	{
		unsigned silenceSize = state->bufferSize - len;
		memset(_audioBuffer + len, 0, silenceSize);
	}


	DSP_FlushDataCache(_audioBuffer, state->bufferSize);
	ndspChnWaveBufAdd(state->handle, &freeBuff);

	clip->playPos += len;

	if (clip->Done())
	{
		state->DequeueClip();
	}

	channel.SwapBuffers();
}
