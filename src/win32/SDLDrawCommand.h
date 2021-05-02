#pragma once

#include <SDL.h>
#include <MathLib.h>
#include "SDL_FontCache.h"

enum class SDLDrawCommandType {
	Sprite = 1,
	Text = 2,
	Line = 3,
	Rectangle = 4
};

struct SDLDrawCommand {
	SDLDrawCommandType type;

	SDL_Texture* texture;
	SDL_Rect src;
	SDL_Rect dst;
	Uint8 r, g, b, a;

	SDL_RendererFlip flip;

	FC_Font* font;
	std::string text;
	float scale;
};