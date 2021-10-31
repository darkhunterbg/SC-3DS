#pragma once

#include "Entity.h"
#include "IEntitySystem.h"
#include "EntityComponentMap.h"

#include "../Data/UnitDef.h"

#include "UnitAIStateMachine.h"
#include "PlayerSystem.h"

struct UnitComponent {
	PlayerId owner;
	const UnitDef* def;
	uint8_t vision = 2;
	uint8_t armor = 0;
	int16_t usedSupply = 0;
	int16_t providedSupply = 0;
	int16_t health;
	int16_t maxHealth;
	int16_t kills = 0;
	uint16_t damage[2] = { 0,0 };
};

struct UnitAIComponent {
	uint8_t stateId = 0;
	bool newState = false;
	uint8_t idleStateId;
};

class UnitSystem : public IEntitySystem {
	// Inherited via IEntitySystem
	virtual void DeleteEntities(std::vector<EntityId>& entities) override;
	virtual size_t ReportMemoryUsage() override;

	EntityComponentMap<UnitComponent> _unitComponents;
	EntityComponentMap<UnitAIComponent> _aiComponents;

	std::vector<UnitAIThinkData> _aiThinkData;
	std::vector<UnitAIEnterStateData> _aiEnterStateData;
public:
	UnitSystem();


	UnitComponent& NewUnit(EntityId id, const UnitDef& def, PlayerId owner);
	inline UnitComponent& GetComponent(EntityId id) { return _unitComponents.GetComponent(id); }
	inline const UnitComponent& GetComponent(EntityId id) const { return _unitComponents.GetComponent(id); }
	inline bool IsUnit(EntityId id) const { return _unitComponents.HasComponent(id); }

	inline const std::vector<UnitComponent>& GetUnits() const { return _unitComponents.GetComponents(); }
	inline const std::vector<EntityId>& GetEntities() const { return _unitComponents.GetEntities(); }

	UnitAIComponent& GetAIComponent(EntityId id) { return _aiComponents.GetComponent(id); }

	void UpdateUnitAI(EntityManager& em);

	void UnitAttackEvent(EntityId id);
};