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

Span<Vertex> vertexBuffer;
C3D_BufInfo vbInfo;

DVLB_s* spriteBatchBlob;
shaderProgram_s spriteBatchProgram;
C3D_AttrInfo spriteBatchAttributeInfo;

void Init();
void Uninit();
void UpdateAudioChannel(NDSAudioChannel& channel);

PrintConsole* console = NULL;

void FatalError(const char* error, ...) {
	if (console == nullptr)
		console = consoleInit(GFX_BOTTOM, nullptr);

	int i = 0;
	for (auto channel : audioChannels) {
		ndspChnSetPaused(i++, true);
	}

	va_list args;
	va_start(args, error);
	char buffer[1024];
	stbsp_vsnprintf(buffer, 1024, error, args);
	va_end(args);

	std::cout << CONSOLE_RED << buffer << CONSOLE_RESET << std::endl;
	std::cout << "Press START to exit";

	while (aptMainLoop()) {
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

void TextureLoadTest() {

	romfsInit();
	gfxInitDefault();
	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);

	char dir[64];
	getcwd(&dir[0], 64);

	if (strcmp(dir, "/") == 0)
		assetDir = "3ds/StarCraft/romfs/";
	else
		assetDir = "StarCraft/romfs/";

	Atlas altases[] = {
		{"bullet_flingy",1},
		{"gui",1},
		{"wirefram" ,2} ,
		{"neutral",5},
		{"terran",7},
		{"protoss",12},
		{"thingy",16},
		{"zerg",24}

	};

	std::vector<std::string> textures;

	for (auto a : altases) {
		for (int i = 0; i < a.subatlases; ++i) {
			std::string tex =  std::string(a.name) + "_" + std::to_string(i) + ".t3x";
			textures.push_back(tex);
		}
	}

	console = consoleInit(GFX_BOTTOM, nullptr);

	printf("free space %iMB / %iMB\n", envGetHeapSize() / (1024 * 1024),  linearSpaceFree() / (1024 * 1024));


	int i = 0;

	size_t texMemoryUsed = 0;

	while (aptMainLoop())
	{
		svcSleepThread(1);
		hidScanInput();

		u32 kDown = hidKeysDown();
		if (kDown & KEY_START)
			break;

		if (i >= textures.size())
			FatalError("Done loading %i textures", textures.size());

		std::string file = assetDir + "atlases/" + textures[i];

		FILE* f = fopen(file.data(), "rb");

		if (f == nullptr)
			FatalError("Failed to open %s", textures[i].data());

		printf("Loading %s (%i/%i)\n", textures[i].data(), ++i, textures.size());

		C3D_Tex tex;
		auto loaded = Tex3DS_TextureImportStdio(f, &tex, nullptr, false);
		if (loaded == nullptr)
			FatalError("Failed to load texture! Used Memory %iMB", texMemoryUsed/(1024*1024));

		texMemoryUsed += tex.size;

		fclose(f);
	}
}

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


		for (auto channel : audioChannels) {
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

void LoadShader() {
	spriteBatchBlob = DVLB_ParseFile((u32*)spritebatch_shbin, spritebatch_shbin_size);
	if (!spriteBatchBlob)
		FatalError("Failed to compile spritebatch shader");

	static constexpr const int VBSize = 64 * 1024;

	vertexBuffer = { (Vertex*)linearAlloc(sizeof(Vertex) * VBSize), VBSize };

	shaderProgramInit(&spriteBatchProgram);
	shaderProgramSetVsh(&spriteBatchProgram, &spriteBatchBlob->DVLE[0]);

	AttrInfo_Init(&spriteBatchAttributeInfo);
	AttrInfo_AddLoader(&spriteBatchAttributeInfo, 0, GPU_FLOAT, 2); // v0=position
	AttrInfo_AddLoader(&spriteBatchAttributeInfo, 1, GPU_FLOAT, 2); // v1=texcoord
	AttrInfo_AddLoader(&spriteBatchAttributeInfo, 2, GPU_UNSIGNED_BYTE, 4); // v2=color

	BufInfo_Init(&vbInfo);
	BufInfo_Add(&vbInfo, vertexBuffer.Data(), sizeof(Vertex), 3, 0x210);
}

void Init() {

	romfsInit();
	gfxInitDefault();
	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);

	// Enable N3DS 804MHz operation, where available
	//osSetSpeedupEnable(true);

	//Result r = APT_SetAppCpuTimeLimit(30);
	//if (r)
	//	FatalError("svcReleaseSemaphore failed with %s", R_SUMMARY(r));


	LoadShader();

	C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
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
		assetDir = "3ds/StarCraft/romfs/";
	else
		assetDir = "StarCraft/romfs/";


	userDir = assetDir.substr(0, assetDir.length() - 6) + "User/";
	struct stat s = { 0 };

	if (stat(userDir.data(), &s) != 0 || !S_ISDIR(s.st_mode)) {
		mkdir(userDir.data(), 0444);
	}

	textBuffer = C2D_TextBufNew(4096);
}
void Uninit() {

	ndspExit();
	C2D_Fini();
	C3D_Fini();
	gfxExit();
	romfsExit();
}

void UpdateAudioChannel(NDSAudioChannel& channel) {

	// TODO: on update check if clip has changed and if so, clear chain and start over 

	auto state = channel.state;

	ndspWaveBuf& usedBuff = *channel.freeBuff;
	ndspWaveBuf& freeBuff = *channel.freeBuff;

	// Wait for playing buffer to finish
	if (usedBuff.status == NDSP_WBUF_QUEUED || usedBuff.status == NDSP_WBUF_PLAYING)
		return;

	AudioChannelClip* clip = state->CurrentClip();
	unsigned size = clip != nullptr ? clip->Remaining() : 0;
	if (size == 0) {
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

	if (len < state->bufferSize) {
		unsigned silenceSize = state->bufferSize - len;
		memset(_audioBuffer + len, 0, silenceSize);
	}


	DSP_FlushDataCache(_audioBuffer, state->bufferSize);
	ndspChnWaveBufAdd(state->handle, &freeBuff);

	clip->playPos += len;

	if (clip->Done()) {
		state->DequeueClip();
	}

	channel.SwapBuffers();
}
