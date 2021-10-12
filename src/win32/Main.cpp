#include <SDL.h>
#include <SDL_gpu.h>

#include <filesystem>
#include <thread>

#include "Color.h"
#include "SDL_FontCache.h"
#include "Game.h"
#include "MathLib.h"

GPU_Image* screens[2];
GPU_Image* white;
GPU_Target* screen;
SDL_Window* window;
std::filesystem::path assetDir;
std::filesystem::path userDir;
uint64_t mainTimer;

Rectangle touchScreenLocation;
bool mute = false;
bool noThreading = true;

void Draw();

int main(int argc, char** argv) {

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_AUDIO);

	GPU_SetRequiredFeatures(GPU_FEATURE_BASIC_SHADERS);
	screen = GPU_Init(400, 480, GPU_DEFAULT_INIT_FLAGS);

	window = SDL_GetWindowFromID(screen->context->windowID);

	SDL_SetWindowResizable(window, SDL_TRUE);
	SDL_SetWindowTitle(window, "StarCraft");
	SDL_MaximizeWindow(window);


	screens[0] = GPU_CreateImage(400, 240, GPU_FORMAT_RGBA);
	screens[1] = GPU_CreateImage(320, 240, GPU_FORMAT_RGBA);
	GPU_LoadTarget(screens[0]);
	GPU_LoadTarget(screens[1]);
	GPU_SetImageFilter(screens[0], GPU_FILTER_LINEAR);
	GPU_SetImageFilter(screens[1], GPU_FILTER_LINEAR);
	GPU_SetShapeBlendMode(GPU_BLEND_NORMAL);
	white = GPU_CreateImage(1, 1, GPU_FORMAT_RGBA);
	uint8_t data[4] = { 255,255,255,255 };
	GPU_Rect rect = { 0,0,1,1 };
	GPU_UpdateImageBytes(white, &rect, data, sizeof(data));

	assetDir = std::filesystem::current_path().parent_path();
	assetDir = assetDir.append("cooked_assets").append("win32");
	userDir = std::filesystem::current_path();
	userDir = userDir.append("User");

	if (!std::filesystem::exists(userDir))
		std::filesystem::create_directories(userDir);

	bool done = false;

	mainTimer = SDL_GetPerformanceCounter();

	Game::Start();

	if (argc > 1) {
		std::string a = argv[1];
		if (a == "-mute")
		{
			mute = true;
		}
	}

	while (!done) {
		Game::FrameStart();

		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type)
			{
			case SDL_QUIT: done = true; break;

			case SDL_WINDOWEVENT:
				if (event.window.event == SDL_WINDOWEVENT_CLOSE)
					done = true;
				if (event.window.event == SDL_WINDOWEVENT_RESIZED)
					GPU_SetWindowResolution(event.window.data1, event.window.data2);
				break;
			}
		}

		if (done)
			break;


		done = !Game::Update();

		if (done)
			break;

		Game::Draw();

		Draw();

		Game::FrameEnd();
		GPU_Flip(screen);
	}

	Game::End();


	GPU_Quit();
	SDL_Quit();

	return 0;
}

void Draw() {

	GPU_DeactivateShaderProgram();


	int w = screen->base_w;
	int h = screen->base_h;

	GPU_Clear(screen);

	const int n3dsUpW = 400;
	const int n3dsUpH = 240;
	const int n3dsDnW = 320;
	const int n3dsDnH = 240;

	SDL_Rect clip = { 0 };

	// Top

	clip.h = h / 2;
	clip.w = (clip.h * n3dsUpW) / n3dsUpH;
	clip.x = (w - clip.w) / 2;

	GPU_Rect r = { (float)clip.x, (float)clip.y, (float)clip.w, (float)clip.h };
	GPU_BlitRect(screens[0], nullptr, screen, &r);

	clip.y = clip.h;
	clip.w = (clip.h * n3dsDnW) / n3dsDnH;
	clip.x = (w - clip.w) / 2;

	touchScreenLocation.position = { clip.x, clip.y };
	touchScreenLocation.size = { clip.w,clip.h };

	r = { (float)clip.x, (float)clip.y, (float)clip.w, (float)clip.h };
	GPU_BlitRect(screens[1], nullptr, screen, &r);

	const Color& c = Colors::CornflowerBlue;

	for (auto s : screens) {

		GPU_ClearRGBA(s->target, (Uint8)(c.r * 255), (Uint8)(c.g * 255), (Uint8)(c.b * 255), 255);

	}
}
