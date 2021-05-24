#include <SDL.h>
#include <filesystem>

#include "Color.h"
#include "lodepng.h"
#include "SDLDrawCommand.h"
#include "SDL_FontCache.h"
#include "Game.h"
#include "MathLib.h"

SDL_Renderer* renderer;
SDL_Texture* screens[2];
SDL_Window* window;
std::filesystem::path assetDir;
uint64_t mainTimer;
Rectangle touchScreenLocation;
bool mute = false;
bool noThreading = true;

void Init();
void Uninit();
void Draw();

int static GetDriver() {
	int d = SDL_GetNumRenderDrivers();

	for (int i = 0; i < d; ++i) {
		SDL_RendererInfo info;
		SDL_GetRenderDriverInfo(i, &info);
		if (info.name == std::string("direct3d11")) /*||
			info.name == std::string("opengl"))*/
			return i;
	}

	return -1;
}

int main(int argc, char** argv) {

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_AUDIO);

	SDL_SetHint(SDL_HINT_RENDER_DRIVER, "direct3d11");

	window = SDL_CreateWindow("StarCraft", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 400, 480,
		SDL_WINDOW_RESIZABLE);
	SDL_MaximizeWindow(window);


	renderer = SDL_CreateRenderer(window, GetDriver(), SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE);

	SDL_SetRenderDrawBlendMode(renderer, SDL_BlendMode::SDL_BLENDMODE_BLEND);

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");

	screens[0] = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 400, 240);
	screens[1] = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 320, 240);

	assetDir = std::filesystem::current_path();

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
				if (event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
					done = true;
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

		SDL_RenderSetViewport(renderer, nullptr);
		SDL_Rect clip = { 0 };
		SDL_GetWindowSize(window, &clip.w, &clip.h);
		SDL_RenderSetClipRect(renderer, &clip);


		SDL_RenderPresent(renderer);
	}

	Game::End();


	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}

void Draw() {
	int w, h;
	SDL_SetRenderTarget(renderer, nullptr);
	SDL_GetRendererOutputSize(renderer, &w, &h);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);


	const int n3dsUpW = 400;
	const int n3dsUpH = 240;
	const int n3dsDnW = 320;
	const int n3dsDnH = 240;

	SDL_Rect clip = { 0 };

	// Top

	clip.h = h / 2;
	clip.w = (clip.h * n3dsUpW) / n3dsUpH;
	clip.x = (w - clip.w) / 2;
	SDL_RenderCopy(renderer, screens[0], nullptr, &clip);

	clip.y = clip.h;
	clip.w = (clip.h * n3dsDnW) / n3dsDnH;
	clip.x = (w - clip.w) / 2;

	touchScreenLocation.position = { clip.x, clip.y };
	touchScreenLocation.size = { clip.w,clip.h };

	SDL_RenderCopy(renderer, screens[1], nullptr, &clip);


	const Color& c = Colors::CornflowerBlue;

	SDL_SetRenderDrawColor(renderer, (Uint8)(c.r * 255), (Uint8)(c.g * 255), (Uint8)(c.b * 255), 255);
	for (auto screen : screens) {
		SDL_SetRenderTarget(renderer, screen);
		SDL_RenderClear(renderer);
	}


}
