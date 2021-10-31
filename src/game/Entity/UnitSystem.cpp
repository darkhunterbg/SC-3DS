#include "UnitSystem.h"

#include "EntityUtil.h"
#include "EntityManager.h"

UnitSystem::UnitSystem()
{
	_aiThinkData.resize(IUnitAIState::States.size());
	_aiThinkData.shrink_to_fit();
	_aiEnterStateData.resize(IUnitAIState::States.size());
	_aiEnterStateData.shrink_to_fit();
}

void UnitSystem::DeleteEntities(std::vector<EntityId>& entities)
{
	_unitComponents.DeleteComponents(entities);
	_aiComponents.DeleteComponents(entities);
}

size_t UnitSystem::ReportMemoryUsage()
{
	size_t size = _unitComponents.GetMemoryUsage();
	size += _aiComponents.GetMemoryUsage();
	for (auto& data : _aiThinkData)
	{
		size += data.entities.capacity() * sizeof(EntityId);
	}

	for (auto& data : _aiEnterStateData)
	{
		size += data.entities.capacity() * sizeof(EntityId);
	}

	return size;
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

	if (def.AI.AIType != UnitAIType::None)
	{
		_aiComponents.NewComponent(id);
		auto& ai = _aiComponents.GetComponent(id);
		ai.stateId = 0;
		ai.newState = true;
		ai.idleStateId = ai.stateId;

	}

	return unit;
}


void UnitSystem::UpdateUnitAI(EntityManager& em)
{
	for (auto& data : _aiEnterStateData)
	{
		data.clear();
	}

	for (auto& data : _aiThinkData)
	{
		data.clear();
	}

	auto& entities = _aiComponents.GetEntities();
	auto& ai = _aiComponents.GetComponents();

	for (int i = 0; i < entities.size(); ++i)
	{
		int stateId = ai[i].stateId;
		EntityId id = entities[i];
		if (ai[i].newState)
		{
			ai[i].newState = false;
			_aiEnterStateData[stateId].entities.push_back(id);
		}

		_aiThinkData[stateId].entities.push_back(id);
	}

	for (int i = 0; i < IUnitAIState::States.size(); ++i)
	{
		auto& state = *IUnitAIState::States[i];
		auto& entierStateData = _aiEnterStateData[i];
		auto& thinkData = _aiThinkData[i];

		if (_aiEnterStateData.size())
			state.EnterState(entierStateData, em);

		if (thinkData.size())
			state.Think(thinkData, em);
	}
}

void UnitSystem::UnitAttackEvent(EntityId id)
{
	UnitComponent& unit = GetComponent(id);
	auto& attack = unit.def->GetAttacks()[0];

	auto sound = attack.GetWeapon()->GetSpawnSound();

	if (sound)
	{
		EntityUtil::GetManager().SoundSystem.PlayWorldSound(*sound, EntityUtil::GetManager().GetPosition(id));
	}
}
