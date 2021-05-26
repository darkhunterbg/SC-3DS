#pragma once

#include "../Entity/Entity.h"
#include "../Assets.h"
#include "../Entity/Common.h"
#include "../Data/RaceDef.h"
#include "../Entity/Component.h"
#include "../Data/AbilityDef.h"
#include <vector>

class EntityManager;

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

	EntityManager* em = nullptr;
	const RaceDef* race = nullptr;

	EntityCollection selection;

	PlayerId player = 1;

	bool IsTargetSelectionMode = false;
	const AbilityDef* currentAbility = nullptr;
	std::vector<GUIActionMarker> markers;

	void ActivateAbility(const AbilityDef* ability);
	void ActivateAbility(const AbilityDef* ability, EntityId target);
	void ActivateAbility(const AbilityDef* ,Vector2Int16 position);

	void ActivateCurrentAbility();
	void ActivateCurrentAbility(EntityId target);
	void ActivateCurrentAbility(Vector2Int16 position);
	
	void SelectAbilityTarget(const AbilityDef& def);
	void CancelTargetSelection();

	void NewActionMarker(Vector2Int16 pos);

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

private:
	void PlayUnitSelectedAudio(UnitChatType type);
};