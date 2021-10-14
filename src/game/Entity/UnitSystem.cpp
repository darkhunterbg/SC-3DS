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
	unit.vision = def.Stats.Vision;
	unit.providedSupply = def.Stats.ProvideSupply;
	unit.usedSupply = def.Stats.UseSupply;

	return unit;
}
