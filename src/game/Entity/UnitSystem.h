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
	uint8_t armorD = 0;
	int16_t usedSupplyD = 0;
	int16_t providedSupplyD = 0;
	int16_t healthD = 0;
	int16_t maxHealthD = 0;
	int16_t kills = 0;
	uint16_t damageD[2] = { 0,0 };
	Vector2Int GetHealth() const { return { (int)healthD >> 1 ,(int)maxHealthD >> 1 }; }
	int GetWeaponDamage(int weapon) { return damageD[weapon] >> 1; };
};

struct UnitAIComponent {
	UnitAIStateId stateId;
	UnitAIStateId idleStateId;
	bool newState = false;

	uint8_t seekRange = 0;
	EntityId targetEntity = Entity::None;
	uint8_t attackId = 0;
	uint8_t attackCooldown = 0;

	bool IsAttackReady() const { return attackCooldown == 0; }
};

class UnitSystem : public IEntitySystem {
	// Inherited via IEntitySystem
	virtual void DeleteEntities(std::vector<EntityId>& entities) override;
	virtual size_t ReportMemoryUsage() override;

	EntityComponentMap<UnitComponent> _unitComponents;
	EntityComponentMap<UnitAIComponent> _aiComponents;

	std::vector<UnitAIState*> _aiStates;
public:
	UnitSystem();
	~UnitSystem();

	UnitComponent& NewUnit(EntityId id, const UnitDef& def, PlayerId owner);
	inline UnitComponent& GetComponent(EntityId id) { return _unitComponents.GetComponent(id); }
	inline const UnitComponent& GetComponent(EntityId id) const { return _unitComponents.GetComponent(id); }
	inline bool IsUnit(EntityId id) const { return _unitComponents.HasComponent(id); }

	inline const std::vector<UnitComponent>& GetUnits() const { return _unitComponents.GetComponents(); }
	inline const std::vector<EntityId>& GetEntities() const { return _unitComponents.GetEntities(); }

	UnitAIComponent& GetAIComponent(EntityId id) { return _aiComponents.GetComponent(id); }

	void UpdateUnitAI(EntityManager& em);

	void UnitAttackEvent(EntityId unit);
};