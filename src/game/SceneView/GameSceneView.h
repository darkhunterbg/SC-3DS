#pragma once

#include "../Entity/PlayerSystem.h"

#include "../Camera.h"
#include "../Widgets/Cursor.h"
#include "../Widgets/ResourceBar.h"
#include "../Widgets/MinimapPanel.h"
#include "../Widgets/SelectionInfoPanel.h"
#include "../Widgets/UnitPortraitPanel.h"
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
	UnitPortraitPanel _unitPortrait;

	PlayerId _player;

	void DrawMainScreen();
	void DrawSecondaryScreen();

	std::vector<EntityId> _temp;
	std::vector<EntityId> _unitSelection;

	bool _cursorOverUI = false;

	Color GetAlliedUnitColor(EntityId id);
	void UpdateSelection();
	void OnEntitiesDeleted(const std::vector<EntityId>& entities);

	int _portraitId = 0;

	void DrawPortrait();
public:
	GameSceneView(GameScene* scene);

	void SetPlayer(PlayerId player);
	inline PlayerId GetPlayer() const { return _player; }
	inline Camera& GetCamera() { return _camera; }
	const PlayerInfo& GetPlayerInfo() const;

	inline const std::vector<EntityId>& GetSelection() const
	{
		return _unitSelection;
	}

	void Update();
	void Draw();

	void OnPlatformChanged();
};