#pragma once

#include "../Entity/PlayerSystem.h"

#include "../Camera.h"
#include "../Widgets/Cursor.h"
#include "../Widgets/ResourceBar.h"
#include "../Widgets/MinimapPanel.h"
#include "../Widgets/SelectionInfoPanel.h"
#include "../Widgets/UnitPortraitPanel.h"
#include "../Widgets/UnitCommandPanel.h"
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
	UnitCommandPanel _commandPanel;

	PlayerId _player;

	const struct AbilityDef* _selectTargetAbility = nullptr;

	void DrawMainScreen();
	void DrawSecondaryScreen();

	std::vector<EntityId> _temp;
	std::vector<EntityId> _unitSelection;

	bool _cursorOverUI = false;
	bool _cursorOverMinimap = false;


	Color GetAlliedUnitColor(EntityId id);
	void UpdateSelection();
	void ContextActionCheck();
	void TargetActionCheck();
	void OnEntitiesDeleted(const std::vector<EntityId>& entities);
	void SetDefaultMode();

	void ActivateAbilityAt(Vector2Int16 worldPos);
	void ActivateContextAbilityAt(Vector2Int16 worldPos);
	void OnAbilityActivated();
	void OnUnitSelect(EntityId id, bool newSelection);

	void DrawPortrait();
	void DrawCommandPanel();
	void DrawMinimap();

	inline	bool IsTargetSelectionMode() const
	{
		return _selectTargetAbility != nullptr;
	}
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

	inline void SetSelection(EntityId id)
	{
		_unitSelection.clear();
		_unitSelection.push_back(id);
		OnUnitSelect(id, true);
	}

	void Update();
	void Draw();

	void OnPlatformChanged();

	EntityId GetEntityUnderCursor();
	EntityId GetEntityUnderPosition(Vector2Int16 worldPos);

	void ActivateAbility(const struct AbilityDef* ability);
	void ActivateAbility(const struct AbilityDef* ability, EntityId target);
	void ActivateAbility(const struct AbilityDef* ability, Vector2Int16 targetPos);
};