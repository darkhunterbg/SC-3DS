#pragma once

#include "../Entity/Entity.h"
#include "../Assets.h"
#include "../Entity/Common.h"
#include "../Data/RaceDef.h"
#include "../Entity/Component.h"
#include "../Data/AbilityDef.h"
#include "../Entity/Common.h"
#include <vector>

class EntityManager;
struct PlayerInfo;

struct GUIActionMarker {
	Vector2Int16 pos;
	uint8_t timer;
	uint8_t state = 0;
};


enum class GameViewUnitFrienldyState {
	Neutral,
	Friendly,
	Enemy
};

class GameViewContext {
public:
	static constexpr const int MarkerTimer = 16;
	static constexpr const int AbilityUnitTargetMarkerTimer = 60;


	EntityManager* em = nullptr;
	const RaceDef* race = nullptr;

	EntityCollection selection;

	PlayerId player = 1;

	bool IsTargetSelectionMode = false;
	const AbilityDef* currentAbility = nullptr;
	EntityId abilityTarget = Entity::None;
	int abilityTargetMarkerTimer = 0;
	std::vector<GUIActionMarker> markers;

	Color selectionColor;

	inline bool IsAbilityTargetMarkerVisible() const {
		return abilityTargetMarkerTimer > 0 &&
			(((AbilityUnitTargetMarkerTimer - abilityTargetMarkerTimer) / 20) % 2 == 0);
	}

	void ActivateAbility(const AbilityDef* ability);
	void ActivateAbility(const AbilityDef* ability, EntityId target);
	void ActivateAbility(const AbilityDef* ability, Vector2Int16 position);
	void ActivateAbility(const AbilityDef* ability, const UnitDef& produce);

	void ActivateCurrentAbility();
	void ActivateCurrentAbility(EntityId target);
	void ActivateCurrentAbility(Vector2Int16 position);
	void ActivateCurrentAbility(const UnitDef& produce);

	void CancelBuildQueue(int queuePos);

	void SelectAbilityTarget(const AbilityDef& def);
	void CancelTargetSelection();

	void NewActionMarker(Vector2Int16 pos);
	void NewUnitMarker(EntityId unit);

	inline EntityManager& GetEntityManager() const {
		return *em;
	}

	inline const SpriteFrameAtlas& GetCommandIconsAtlas() const {
		return *race->CommandIconsAtlas;
	}

	void SelectUnitsInRegion(Rectangle16 region);

	void SelectUnitAtPosition(Vector2Int16 position);

	EntityId GetUnitAtPosition(Vector2Int16 position);

	bool IsVisibleUnit(EntityId id);

	EntityId GetPriorityUnitSelected() const;

	bool HasSelectionControl() const;

	Color GetAlliedUnitColor(EntityId id) const;

	void PlayUnitSelectedAudio(UnitChatType type);

	const RaceDef& GetPlayerRaceDef() const;

	const PlayerInfo& GetPlayerInfo() const;
};