#include "UnitSystem.h"

void UnitSystem::DeleteEntities(std::vector<EntityId>& entities)
{
	_unitComponents.DeleteComponents(entities);
}

size_t UnitSystem::ReportMemoryUsage()
{
	return _unitComponents.GetMemoryUsage();
}

UnitComponent& UnitSystem::NewUnit(EntityId id, const UnitDef& def, PlayerId owner)
{
	_unitComponents.NewComponent(id);

	UnitComponent& unit = _unitComponents.GetComponent(id);

	unit.owner = owner;
	unit.def = &def;
	unit.vision = def.Stats.Vision + 1;
	unit.armor = def.Combat.Armor;
	unit.providedSupply = def.Stats.ProvideSupply;
	unit.usedSupply = def.Stats.UseSupply;
	unit.health = unit.maxHealth = def.Stats.Health;
	unit.damage[0] = def.Attacks[0].Damage;
	unit.damage[1] = def.Attacks[1].Damage;
	return unit;
}
