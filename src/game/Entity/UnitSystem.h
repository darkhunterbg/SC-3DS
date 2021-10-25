#pragma once

#include "Entity.h"
#include "IEntitySystem.h"
#include "EntityComponentMap.h"

#include "../Data/UnitDef.h"

#include "PlayerSystem.h"

struct UnitComponent {
	PlayerId owner;
	const UnitDef* def;
	uint8_t vision = 2;
	int16_t usedSupply = 0;
	int16_t providedSupply = 0;
	int16_t health;
	int16_t maxHealth;
	int16_t kills = 0;
};

class UnitSystem : public IEntitySystem {
	// Inherited via IEntitySystem
	virtual void DeleteEntities(std::vector<EntityId>& entities) override;
	virtual size_t ReportMemoryUsage() override;

	EntityComponentMap<UnitComponent> _unitComponents;

public:

	UnitComponent& NewUnit(EntityId id, const UnitDef& def, PlayerId owner);
	inline UnitComponent& GetComponent(EntityId id) { return _unitComponents.GetComponent(id); }
	inline const UnitComponent& GetComponent(EntityId id) const { return _unitComponents.GetComponent(id); }
	inline bool IsUnit(EntityId id) const { return _unitComponents.HasComponent(id); }

	inline const std::vector<UnitComponent>& GetUnits() const { return _unitComponents.GetComponents(); }
	inline const std::vector<EntityId>& GetEntities() const { return _unitComponents.GetEntities(); }
};