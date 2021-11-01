#include "UnitSystem.h"

#include "EntityUtil.h"
#include "EntityManager.h"

UnitSystem::UnitSystem()
{
	UnitAIStateMachine::CreateStates(_aiStates);
}

UnitSystem::~UnitSystem()
{
	for (auto state : _aiStates)
		delete state;
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
	for (auto& state : _aiStates)
	{
		size += state->GetSize();
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
	unit.armorD = def.Combat.Armor << 1;
	unit.providedSupplyD = def.Stats.ProvideSupply;
	unit.usedSupplyD = def.Stats.UseSupply;
	unit.healthD = unit.maxHealthD = (def.Stats.Health << 1);
	unit.damageD[0] = def.Attacks[0].Damage << 1;
	unit.damageD[1] = def.Attacks[1].Damage << 1;

	if (def.AI.AIType != UnitAIType::None)
	{
		_aiComponents.NewComponent(id);
		auto& ai = _aiComponents.GetComponent(id);
		ai.stateId = UnitAIStateId::IdleAggressive;
		ai.newState = true;
		ai.idleStateId = ai.stateId;
		ai.seekRange = def.GetAttacks()[0].Range.y;
	}

	return unit;
}


void UnitSystem::UpdateUnitAI(EntityManager& em)
{
	for (auto& state : _aiStates)
	{
		state->enterStateData.clear();
		state->thinkData.clear();
	}


	auto& entities = _aiComponents.GetEntities();
	auto& ai = _aiComponents.GetComponents();

	for (int i = 0; i < entities.size(); ++i)
	{
		UnitAIStateId stateId = ai[i].stateId;
		EntityId id = entities[i];
		if (ai[i].newState)
		{
			ai[i].newState = false;
			_aiStates[(int)stateId]->enterStateData.entities.push_back(id);
		}

		ai[i].attackCooldown -= ai[i].attackCooldown > 0;

		_aiStates[(int)stateId]->thinkData.entities.push_back(id);
	}

	// Todo: have 2 states list, for enter state and for think to avid ifs

	for (UnitAIState* state : _aiStates)
	{
		if (state->enterStateFunc && state->enterStateData.size())
			state->enterStateFunc(state->enterStateData, em);

		if (state->thinkFunc && state->thinkData.size())
			state->thinkFunc(state->thinkData, em);
	}
}

void UnitSystem::UnitAttackEvent(EntityId id)
{
	UnitComponent& unit = GetComponent(id);
	UnitAIComponent& ai = GetAIComponent(id);

	auto& attack = unit.def->GetAttacks()[ai.attackId];
	auto sound = attack.GetWeapon()->GetSpawnSound();

	if (sound)
	{
		EntityUtil::GetManager().SoundSystem.PlayWorldSound(*sound, EntityUtil::GetManager().GetPosition(id));
	}

	UnitComponent& target = GetComponent(id);
	target.healthD -= unit.damageD[ai.attackId];
	ai.attackCooldown = attack.Cooldown;
}
