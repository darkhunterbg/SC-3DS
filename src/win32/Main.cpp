#include <SDL.h>
#include <filesystem>

#include "Color.h"
#include "lodepng.h"
#include "SDLDrawCommand.h"
#include "SDL_FontCache.h"
#include "Game.h"
#include "MathLib.h"

SDL_Renderer* renderer;
SDL_Window* window;
std::filesystem::path assetDir;
std::vector<SDLDrawCommand> commandBuffers[2];
uint64_t mainTimer;
Rectangle touchScreenLocation;
bool mute = false;

void Init();
void Uninit();
void Draw();

void ExecDrawCommand(const SDLDrawCommand& cmd, SDL_Rect& clip, const int& n3dsUpW, const int& n3dsUpH);

int main(int argc, char** argv) {

	float u = atan2f(-100, 0);
	float r = atan2f(0, 100);
	float d = atan2f(100, 0);
	float l = atan2f(0, -100);

	Vector2Int p = { -100,100 };
	float angle = atan2f(-p.y, p.x) + PI / 2.0f;
	int mapped = ((int)(((angle * 16) / PI) + 32) % 32);

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_AUDIO);
	window = SDL_CreateWindow("StarCraft", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1440, 720, SDL_WINDOW_RESIZABLE);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BlendMode::SDL_BLENDMODE_ADD);

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


		SDL_RenderSetViewport(renderer, nullptr);
		SDL_Rect clip = { 0 };
		SDL_GetWindowSize(window, &clip.w, &clip.h);
		SDL_RenderSetClipRect(renderer, &clip);

		Game::FrameEnd();

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
	SDL_GetRendererOutputSize(renderer, &w, &h);

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	//SDL_SetRenderDrawColor(ren, 0, 140, 150, 255);
	SDL_RenderClear(renderer);

	const Color& c = Colors::CornflowerBlue;
	SDL_SetRenderDrawColor(renderer, (Uint8)(c.r * 255), (Uint8)(c.g * 255), (Uint8)(c.b * 255), 255);

	const int n3dsUpW = 400;
	const int n3dsUpH = 240;
	const int n3dsDnW = 320;
	const int n3dsDnH = 240;

	SDL_Rect clip = { 0 };

	// Top

	clip.h = h / 2;
	clip.w = (clip.h * n3dsUpW) / n3dsUpH;
	clip.x = (w - clip.w) / 2;

	SDL_RenderSetViewport(renderer, &clip);
	clip.x = 0;
	SDL_RenderFillRect(renderer, &clip);


	for (const SDLDrawCommand& cmd : commandBuffers[0]) {
		ExecDrawCommand(cmd, clip, n3dsUpW, n3dsUpH);
	}

	commandBuffers[0].clear();

	// Bottom

	clip.y = clip.h;
	clip.w = (clip.h * n3dsDnW) / n3dsDnH;
	clip.x = (w - clip.w) / 2;

	touchScreenLocation.position = { clip.x, clip.y };
	touchScreenLocation.size = { clip.w,clip.h };

	SDL_RenderSetViewport(renderer, &clip);
	clip.x = 0;
	clip.y = 0;
	SDL_RenderFillRect(renderer, &clip);

	for (const SDLDrawCommand& cmd : commandBuffers[1]) {
		ExecDrawCommand(cmd, clip, n3dsDnW, n3dsUpH);
	}
	commandBuffers[1].clear();

}

void ExecDrawCommand(const SDLDrawCommand& cmd, SDL_Rect& clip, const int& w, const int& h)
{
	switch (cmd.type)
	{
	case SDLDrawCommandType::Sprite: {

		SDL_Rect rect = cmd.dst;

		rect.x = (clip.w * rect.x) / w;
		rect.y = (clip.h * rect.y) / h;
		rect.w = (clip.w * rect.w) / w;
		rect.h = (clip.h * rect.h) / h;

		SDL_SetTextureAlphaMod(cmd.texture, cmd.a);
		if (cmd.r != 0 || cmd.g != 0 || cmd.b != 0 || cmd.a != 255) {

			SDL_SetTextureBlendMode(cmd.texture, SDL_BlendMode::SDL_BLENDMODE_BLEND);
			SDL_SetTextureColorMod(cmd.texture, cmd.r, cmd.g, cmd.b);
		}
		else {
			SDL_SetTextureBlendMode(cmd.texture, SDL_BlendMode::SDL_BLENDMODE_BLEND);
			SDL_SetTextureColorMod(cmd.texture, 255, 255, 255);
		}


		SDL_RenderCopyEx(renderer, cmd.texture, &cmd.src, &rect, 0, nullptr, cmd.flip);
		break;
	}
	case SDLDrawCommandType::Text: {
		float x = (clip.w * cmd.dst.x) / w;
		float y = (clip.h * cmd.dst.y) / h;
		float s = (clip.w * cmd.scale) / (float)w;

		FC_DrawScaleColor(cmd.font, renderer, x, y, FC_MakeScale(s, s), FC_MakeColor(cmd.r, cmd.g, cmd.b, cmd.a), cmd.text.data());

		break;
	}
	case SDLDrawCommandType::Line: {

		Vector2Int src = { cmd.src.x, cmd.src.y };
		src.x = (clip.w * src.x) / w;
		src.y = (clip.h * src.y) / h;

		Vector2Int dst = { cmd.dst.x, cmd.dst.y };
		dst.x = (clip.w * dst.x) / w;
		dst.y = (clip.h * dst.y) / h;

		int thickness = std::max(1, (clip.w) / w);


		SDL_SetRenderDrawColor(renderer, cmd.r, cmd.g, cmd.b, cmd.a);
		SDL_RenderDrawLine(renderer, src.x, src.y, dst.x, dst.y);
		for (int i = 1; i < thickness; ++i) {
			//  -
			//	_
			//	|
			//		|
			SDL_RenderDrawLine(renderer, src.x - i, src.y - i, dst.x + i, dst.y - i);
			SDL_RenderDrawLine(renderer, src.x - i, src.y + i, dst.x + i, dst.y + i);
			SDL_RenderDrawLine(renderer, src.x - i, src.y - i, dst.x - i, dst.y + i);
			SDL_RenderDrawLine(renderer, src.x + i, src.y - i, dst.x + i, dst.y + i);
		}
		break;
	}
	case SDLDrawCommandType::Rectangle: {

		SDL_Rect rect = cmd.dst;

		rect.x = (clip.w * rect.x) / w;
		rect.y = (clip.h * rect.y) / h;
		rect.w = (clip.w * rect.w) / w;
		rect.h = (clip.h * rect.h) / h;

		SDL_SetRenderDrawColor(renderer, cmd.r, cmd.g, cmd.b, cmd.a);
		SDL_RenderFillRect(renderer, &rect);
		break;
	}
	}


}
