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
	struct ActionMarker {
		Vector2Int16 pos;
		uint8_t timer;
		uint8_t state = 0;
	};

	struct UnitMarker {
		EntityId id;
		int timer;
	};

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
	std::vector<ActionMarker> _markers;
	UnitMarker _unitMarker = { Entity::None, 0 };

	bool _cursorOverUI = false;
	bool _cursorOverMinimap = false;

	const struct SoundSetDef* _selectSound = nullptr;

	Color GetAlliedUnitColor(EntityId id);
	void UpdateSelection();
	void ContextActionCheck();
	void TargetActionCheck();
	void OnEntitiesDeleted(const std::vector<EntityId>& entities);
	void SetDefaultMode();

	void ActivateAbilityAt(Vector2Int16 worldPos);
	void ActivateContextAbilityAt(Vector2Int16 worldPos);

	inline void OnAbilityActivated(Vector2Int16 worldPos) { OnAbilityActivated(Entity::None, worldPos); }
	void OnAbilityActivated(EntityId target = Entity::None, Vector2Int16 worldPos = { -1,-1 });
	void OnUnitSelect(EntityId id, bool newSelection);

	void DrawPortrait();
	void DrawCommandPanel();
	void DrawMinimap();

	void DrawMarkers();

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
};