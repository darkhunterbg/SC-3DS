#pragma once

#include "MathLib.h"

#include <vector>
#include <SDL.h>
#include <SDL_gpu.h>

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
};

class AbstractPlatform {

public:
	std::vector<AbstractScreen> Screens;
	AbstractPointer Pointer;

	void ApplyPlatform();

	void UpdateScreens(GPU_Target* screen);
	void UpdateInput();
	void Draw(GPU_Target* screen);

	static AbstractPlatform N3DS(GPU_Target* screen)
	{
		AbstractPlatform p;
		p.Pointer.ScreenReference = 1;
		p.Screens.push_back({ {400,240} });
		p.Screens.push_back({ {320,240} });

		return p;
	}

	static AbstractPlatform PC(GPU_Target* screen)
	{
		AbstractPlatform p;
		p.Pointer.SameAsCursor = true;
		p.Screens.push_back({ {640,480} });

		//AbstractScreen s;
		//s.IsScreenReference = true;
		//s.ScreenRef = screen;

		//p.Screens.push_back(s);

		return p;
	}
};