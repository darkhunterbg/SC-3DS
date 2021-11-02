#include "UnitSystem.h"

#include "EntityUtil.h"
#include "EntityManager.h"
#include "../Debug.h"

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

	size += _unitAttackEvents.capacity() + _unitDieEvents.capacity() * sizeof(EntityId);

	return size;
}

UnitComponent& UnitSystem::NewUnit(EntityId id, const UnitDef& def, PlayerId owner)
{
	_unitComponents.NewComponent(id);

	UnitComponent& unit = _unitComponents.GetComponent(id);

	unit.owner = owner;
	unit.def = &def;
	unit.vision = def.Stats.Vision + 1;
	unit.armor.SetToInt(def.Combat.Armor);
	unit.providedSupply = def.Stats.ProvideSupply;
	unit.usedSupply = def.Stats.UseSupply;
	unit.health = unit.maxHealth.SetToInt(def.Stats.Health);
	unit.damage[0].SetToInt(def.Attacks[0].Damage);
	unit.damage[1].SetToInt(def.Attacks[1].Damage);

	if (def.AI.AIType != UnitAIType::None)
	{
		_aiComponents.NewComponent(id);
		auto& ai = _aiComponents.GetComponent(id);
		ai.stateId = UnitAIStateId::IdleAggressive;
		ai.newState = true;
		ai.idleStateId = ai.stateId;
		ai.seekRange = def.GetAttacks()[0].MaxRange;
	}

	return unit;
}


void UnitSystem::PrepareUnitAI(EntityManager& em)
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

	for (auto& state : _aiStates)
	{
		state->StartEnterState(0);
		state->StartThink(0);
	}

	_aiUpdatesCompleted = 0;
}

bool UnitSystem::UpdateUnitAI(EntityManager& em)
{
	// Todo: have 2 states list, for enter state and for think to avid ifs

	int batch = 64;

	UnitAIState* state = _aiStates[_aiUpdatesCompleted];

	if (state->enterStateFunc && !state->IsEnterStateCompleted())
	{
		state->AdvanceEnterState(batch);
		state->enterStateFunc(state->enterStateData, em);

		return false;
	}

	if (state->thinkFunc && !state->IsThinkCompleted())
	{
		state->AdvanceThink(batch);
		state->thinkFunc(state->thinkData, em);

		return false;
	}

	++_aiUpdatesCompleted;

	return _aiUpdatesCompleted == _aiStates.size();
}


void UnitSystem::ProcessUnitEvents(EntityManager& em)
{
	for (EntityId id : _unitAttackEvents)
	{
		UnitComponent& unit = GetComponent(id);
		UnitAIComponent& ai = GetAIComponent(id);

		auto& attack = unit.def->GetAttacks()[ai.attackId];
		auto sound = attack.GetWeapon()->GetSpawnSound();

		if (sound)
		{
			EntityUtil::GetManager().SoundSystem.PlayWorldSound(*sound, EntityUtil::GetManager().GetPosition(id));
		}

		UnitComponent& target = GetComponent(ai.targetEntity);

		if (target.IsDead()) continue;

		auto damage = unit.damage[ai.attackId] - unit.armor;
		target.health -= std::max(0, (int)damage.value);
		ai.attackCooldown = attack.Cooldown;

		if (target.IsDead())
		{
			_unitDieEvents.push_back(ai.targetEntity);
			++unit.kills;
		}
	}

	_unitAttackEvents.clear();


	if (_unitDieEvents.size())
	{
		for (EntityId id : _unitDieEvents)
		{
			const UnitDef* def = GetComponent(id).def;

			const AnimClipDef* deathAnim = def->Art.GetSprite().GetAnimation(AnimationType::Death);
			if (deathAnim != nullptr)
			{

				EntityId death = em.NewEntity();
				em.DrawSystem.NewComponent(death);
				em.AnimationSystem.NewComponent(death);
				em.SetPosition(death, em.GetPosition(id));
				em.SetOrientation(death, 0);
				// TODO, fix shadow
				EntityUtil::PlayAnimation(death, *deathAnim, nullptr);
			}
		}

		em.DeleteEntities(_unitDieEvents);

		_unitDieEvents.clear();
	}
}

void UnitSystem::UnitKillEvent(EntityId id)
{
	GAME_ASSERT(_aiComponents.HasComponent(id), "[UnitAttackEvent] Entity %i does not have AI!");
	GAME_ASSERT(_unitComponents.HasComponent(id), "[UnitAttackEvent] Entity %i is not an unit!");

	if (std::find(_unitDieEvents.begin(), _unitDieEvents.end(), id) == _unitDieEvents.end())
		_unitDieEvents.push_back(id);
}


void UnitSystem::UnitAttackEvent(EntityId id)
{
	GAME_ASSERT(_aiComponents.HasComponent(id), "[UnitAttackEvent] Entity %i does not have AI!");
	GAME_ASSERT(_unitComponents.HasComponent(id), "[UnitAttackEvent] Entity %i is not an unit!");

	if (std::find(_unitAttackEvents.begin(), _unitAttackEvents.end(), id) == _unitAttackEvents.end())
		_unitAttackEvents.push_back(id);
}

