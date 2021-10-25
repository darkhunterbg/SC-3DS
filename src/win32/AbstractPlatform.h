#pragma once

#include "MathLib.h"

#include <vector>
#include <SDL.h>
#include <SDL_gpu.h>
#include <string>

#include "Platform.h"

struct AbstractScreen {
	Vector2Int16 Resolution;

	Vector2Int NativePosition;
	Vector2Int NativeResolution;

	bool IsScreenReference = false;
	GPU_Image* Image = nullptr;
	GPU_Target* ScreenRef = nullptr;
};

struct AbstractPointer {
	bool SameAsCursor = false;
	Vector2Int Position;
	int ScreenReference = 0;
	bool InsideScreen = false;
};

class AbstractPlatform {

public:
	std::vector<AbstractScreen> Screens;
	AbstractPointer Pointer;
	std::string Name;
	PlatformType Type = PlatformType::Generic;

	void ApplyPlatform();

	void UpdateScreens(GPU_Target* screen);
	void UpdateInput(SDL_Window* window);
	void Draw(GPU_Target* screen);

	static std::vector<AbstractPlatform> Platforms;
	static std::vector<const char*> PlatformsString;
	
	static void InitPlatforms(GPU_Target* screen);

	static AbstractPlatform N3DS(GPU_Target* screen)
	{
		AbstractPlatform p;
		p.Name = "Nintendo 3DS";
		p.Type = PlatformType::Nintendo3DS;
		p.Pointer.ScreenReference = 1;
		p.Screens.push_back({ {400,240} });
		p.Screens.push_back({ {320,240} });

		return p;
	}

	static AbstractPlatform PC(GPU_Target* screen)
	{
		AbstractPlatform p;
		p.Name = "Desktop";
		p.Pointer.SameAsCursor = true;
		p.Screens.push_back({ {640,480} });

		//AbstractScreen s;
		//s.IsScreenReference = true;
		//s.ScreenRef = screen;

		//p.Screens.push_back(s);

		return p;
	}
};