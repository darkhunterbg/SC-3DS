#include <3ds.h>
#include <citro3d.h>
#include <citro2d.h>
#include <unistd.h>

#include <string>
#include <iostream>

#include "Color.h"
#include "Game.h"
#include "StringLib.h"


C3D_RenderTarget* top;
C3D_RenderTarget* bottom;
C3D_RenderTarget* screens[2];
std::string assetDir;
C2D_TextBuf textBuffer;
u64 mainTimer;

void Init();
void Uninit();

void FatalError(const char* error,...) {
	consoleInit(GFX_BOTTOM, nullptr);

	va_list args;
	va_start(args, error);
	char buffer[1024];
	stbsp_vsnprintf(buffer, 1024, error, args);

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

int main()
{
	Init();

	mainTimer = svcGetSystemTick();

	Game::Start();

	while (aptMainLoop())
	{
		hidScanInput();

		u32 kDown = hidKeysDown();
		if (kDown & KEY_START)
			break;

		bool done = !Game::Update();

		if (done)
			break;

		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

		u32 color = C2D_Color32f(Colors::CornflowerBlue.r, Colors::CornflowerBlue.g, Colors::CornflowerBlue.b, Colors::CornflowerBlue.a);
		C2D_TargetClear(top, color);
		C2D_TargetClear(bottom, color);

		C2D_Prepare();
		Game::Draw();
		C3D_FrameEnd(0);

	}

	Game::End();

	Uninit();

	return 0;
}


void Init() {

	romfsInit();
	gfxInitDefault();

	// Enable N3DS 804MHz operation, where available
	//osSetSpeedupEnable(true);

	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
	C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
	C2D_Prepare();

	screens[0] = top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
	screens[1] = bottom = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);

	ndspInit();
	ndspSetOutputMode(NDSP_OUTPUT_STEREO);

	char dir[64];
	getcwd(&dir[0], 64);

	if (strcmp(dir, "/") == 0)
		assetDir = "3ds/StarCraft/romfs/";
	else
		assetDir = "StarCraft/romfs/";


	textBuffer = C2D_TextBufNew(4096);
}
void Uninit() {

	ndspExit();
	C2D_Fini();
	C3D_Fini();
	gfxExit();
	romfsExit();
}