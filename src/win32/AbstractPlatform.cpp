#include "AbstractPlatform.h"
#include "Color.h"

#include <SDL.h>
#include <SDL_gpu.h>

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
}

void AbstractPlatform::Draw(GPU_Target* screen)
{
	GPU_DeactivateShaderProgram();


	int w = screen->base_w;
	int h = screen->base_h;

	GPU_Clear(screen);

	SDL_Rect clip = { 0 };

	clip.h = h / Screens.size();

	for (AbstractScreen& aScreen : Screens)
	{
		int screenW = aScreen.Resolution.x;
		int sreeenH = aScreen.Resolution.y;

		clip.w = (clip.h * screenW) / sreeenH;
		clip.x = (w - clip.w) / 2;

		GPU_Rect r = { (float)clip.x, (float)clip.y, (float)clip.w, (float)clip.h };
		GPU_BlitRect(aScreen.Image, nullptr, screen, &r);

		clip.y += clip.h;
	}



	//touchScreenLocation.position = { clip.x, clip.y };
	//touchScreenLocation.size = { clip.w,clip.h };

	const Color& c = Colors::CornflowerBlue;

	for (AbstractScreen& aScreen : Screens)
	{
		GPU_ClearRGBA(aScreen.Image->target, (Uint8)(c.r * 255), (Uint8)(c.g * 255), (Uint8)(c.b * 255), 255);
	}
}
