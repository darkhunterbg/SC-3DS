#pragma once

#include "ViewContext.h"
#include "../Camera.h"

class GameView {
private:
	GameView(const GameView&) = delete;
	GameView& operator=(const GameView&) = delete;


	ViewContext _context;
	Camera _camera;
public:
	GameView();

	void Init();

	void SetPlayer(PlayerId player);

	const inline ViewContext& GetContext() { return _context; }
	inline Camera& GetCamera() { return _camera; }

	void DrawMainScreen();
	void DrawSecondaryScreen();
};