#pragma once

#include "MathLib.h"

#include <vector>
#include <SDL.h>
#include <SDL_gpu.h>

struct AbstractScreen {
	Vector2Int16 Resolution;
	bool IsScreenReference = false;
	GPU_Image* Image = nullptr;
	GPU_Target* ScreenRef = nullptr;
};

class AbstractPlatform {

public:
	std::vector<AbstractScreen> Screens;

	void ApplyPlatform();

	void Draw(GPU_Target* screen);

	static AbstractPlatform N3DS()
	{
		AbstractPlatform p;
		p.Screens.push_back({ {400,240} });
		p.Screens.push_back({ {320,240} });

		return p;
	}

	static AbstractPlatform PC(GPU_Target* screen)
	{
		AbstractPlatform p;

		//p.Screens.push_back({ {640,480} });

		AbstractScreen s;
		s.IsScreenReference = true;
		s.ScreenRef = screen;

		p.Screens.push_back(s);

		return p;
	}
};