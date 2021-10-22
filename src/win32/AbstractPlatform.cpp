#include "AbstractPlatform.h"
#include "Color.h"
#include <algorithm>

#include <SDL.h>
#include <SDL_gpu.h>

std::vector<AbstractPlatform> AbstractPlatform::Platforms;
std::vector<const char*> AbstractPlatform::PlatformsString;

void AbstractPlatform::ApplyPlatform()
{
	for (AbstractScreen& screen : Screens)
	{
		if (screen.Image != nullptr)
		{
			GPU_FreeTarget(screen.Image->target);
			GPU_FreeImage(screen.Image);
			screen.Image = nullptr;
		}

		if (screen.IsScreenReference)
		{
			screen.Resolution = { (short)screen.ScreenRef->w, (short)screen.ScreenRef->h };
		}

		screen.Image = GPU_CreateImage(screen.Resolution.x, screen.Resolution.y, GPU_FORMAT_RGBA);

		GPU_LoadTarget(screen.Image);
		GPU_SetImageFilter(screen.Image, GPU_FILTER_LINEAR);

	}

	SDL_ShowCursor(!Pointer.SameAsCursor);
}

void AbstractPlatform::UpdateScreens(GPU_Target* screen)
{
	SDL_Rect clip = { 0 };


	int w = screen->base_w;
	int h = screen->base_h;

	clip.h = h / Screens.size();

	for (AbstractScreen& aScreen : Screens)
	{
		int screenW = aScreen.Resolution.x;
		int sreeenH = aScreen.Resolution.y;

		clip.w = (clip.h * screenW) / sreeenH;
		clip.x = (w - clip.w) / 2;

		aScreen.NativePosition = { clip.x, clip.y };
		aScreen.NativeResolution = { clip.w, clip.h };

		clip.y += clip.h;
	}

}

void AbstractPlatform::UpdateInput()
{
	Vector2Int pos;
	Uint32  buttonState = SDL_GetMouseState(&pos.x, &pos.y);

	auto& screen = Screens[Pointer.ScreenReference];
	pos -= screen.NativePosition;


	//state.Touch = buttonState & SDL_BUTTON(SDL_BUTTON_LEFT);
	Vector2Int nativeSize = screen.NativeResolution;

	pos.x = std::min(std::max(pos.x, 0), nativeSize.x);
	pos.y = std::min(std::max(pos.y, 0), nativeSize.y);
	pos.x = (pos.x * screen.Resolution.x) / nativeSize.x;
	pos.y = (pos.y * screen.Resolution.y) / nativeSize.y;

	Pointer.Position = pos;
}

void AbstractPlatform::Draw(GPU_Target* screen)
{
	GPU_DeactivateShaderProgram();

	UpdateScreens(screen);

	GPU_ClearColor(screen, { 30,30,30,255 });

	SDL_Rect clip = { 0 };

	const Color& c = Colors::CornflowerBlue;

	for (AbstractScreen& aScreen : Screens)
	{
		clip.x = aScreen.NativePosition.x;
		clip.y = aScreen.NativePosition.y;
		clip.w = aScreen.NativeResolution.x;
		clip.h = aScreen.NativeResolution.y;

		GPU_Rect r = { (float)clip.x, (float)clip.y, (float)clip.w, (float)clip.h };
		GPU_BlitRect(aScreen.Image, nullptr, screen, &r);
		GPU_ClearRGBA(aScreen.Image->target, (Uint8)(c.r * 255), (Uint8)(c.g * 255), (Uint8)(c.b * 255), 255);
	}
}

void AbstractPlatform::InitPlatforms(GPU_Target* screen)
{
	Platforms.push_back(PC(screen));
	Platforms.push_back(N3DS(screen));

	for (const auto& p : Platforms)
	{
		PlatformsString.push_back(p.Name.data());
	}
}
