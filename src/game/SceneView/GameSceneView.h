#pragma once

#include "../Entity/PlayerSystem.h"

#include "../Camera.h"
#include "Cursor.h"

class GameScene;

class GameSceneView {
private:
	GameScene* _scene = nullptr;

	Camera _camera;
	Cursor _cursor;

	PlayerId _player;
public:
	GameSceneView(GameScene* scene);

	void SetPlayer(PlayerId player);
	inline PlayerId GetPlayer() const { return _player; }
	inline Camera& GetCamera() { return _camera; }
	const PlayerInfo& GetPlayerInfo() const;

	void Update();
	void DrawMainScreen();
	void DrawSecondaryScreen();
};