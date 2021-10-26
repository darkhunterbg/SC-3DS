#pragma once

#include "../Entity/PlayerSystem.h"

#include "../Camera.h"
#include "../Widgets/Cursor.h"
#include "../Widgets/ResourceBar.h"
#include "../Widgets/MinimapPanel.h"
#include "../Widgets/SelectionInfoPanel.h"
#include "../Entity/Entity.h"

class GameScene;

class GameSceneView {
private:
	GameScene* _scene = nullptr;

	Camera _camera;
	Cursor _cursor;
	ResourceBar _resourceBar;
	MinimapPanel _minimap;
	SelectionInfoPanel _selectionPanel;

	PlayerId _player;

	void DrawMainScreen();
	void DrawSecondaryScreen();

	std::vector<EntityId> _temp;
	std::vector<EntityId> _unitSelection;

	bool _cursorOverUI = false;

	Color GetAlliedUnitColor(EntityId id);
	void UpdateSelection();
public:
	GameSceneView(GameScene* scene);

	void SetPlayer(PlayerId player);
	inline PlayerId GetPlayer() const { return _player; }
	inline Camera& GetCamera() { return _camera; }
	const PlayerInfo& GetPlayerInfo() const;

	void Update();
	void Draw();

	void OnPlatformChanged();
};