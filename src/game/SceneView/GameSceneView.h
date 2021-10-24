#pragma once

#include "../Entity/PlayerSystem.h"

#include "../Camera.h"
#include "Cursor.h"

class GameScene;

class GameSceneView {
private:
	struct Resource {
		int shown;
		int target;
	};

	struct Supply {
		int current;
		int max;
	};

	char textBuffer[128];
	Resource minerals = { 0,0 };
	Resource gas = { 0,0 };
	Supply supply = { 0,0 };

	GameScene* _scene = nullptr;

	Camera _camera;
	Cursor _cursor;

	PlayerId _player;

	void UpdateResourceDiff(GameSceneView::Resource& r);
	void DrawMinimap();
	void DrawResource(const ImageFrame& icon, Vector2Int pos, Color color, const char* fmt, ...);
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