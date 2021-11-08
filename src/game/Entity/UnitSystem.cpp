#include "UnitSystem.h"

#include "EntityUtil.h"
#include "EntityManager.h"
#include "../Data/GameDatabase.h"
#include "../Debug.h"

static constexpr const uint8_t ShieldRegen = 7;
static constexpr const uint8_t EnergyRegen = 8;
static constexpr const uint8_t HealthRegen = 4;
static constexpr const uint8_t StructureBurning = 20;

UnitSystem::UnitSystem()
{
	UnitAIStateMachine::CreateStates(_aiStates);
	UnitStateMachine::CreateStates(_unitStates);
}

UnitSystem::~UnitSystem()
{
	for (auto state : _aiStates)
		delete state;

	for (auto state : _unitStates)
		delete state;
}

void UnitSystem::DeleteEntities(std::vector<EntityId>& entities)
{
	_unitComponents.DeleteComponents(entities);
	_unitStateComponents.DeleteComponents(entities);
	_aiComponents.DeleteComponents(entities);
}

size_t UnitSystem::ReportMemoryUsage()
{
	size_t size = _unitComponents.GetMemoryUsage();
	size += _aiComponents.GetMemoryUsage();
	size += _unitStateComponents.GetMemoryUsage();

	for (auto& state : _aiStates)
	{
		size += state->GetSize();
	}

	for (auto& state : _unitStates)
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
	unit.shield = unit.maxShield.SetToInt(def.Stats.Sheild);
	unit.damage[0].SetToInt(def.Attacks[0].Damage);
	unit.damage[1].SetToInt(def.Attacks[1].Damage);
	unit.attackedBy = Entity::None;

	_unitStateComponents.NewComponent(id);
	auto& state = _unitStateComponents.GetComponent(id);
	state.stateId = UnitStateId::Idle;
	state.newState = true;

	if (def.AI.AIType != UnitAIType::None)
	{
		_aiComponents.NewComponent(id);
		auto& ai = _aiComponents.GetComponent(id);
		ai.stateId = UnitAIStateId::Idle;
		ai.seekRange = def.AI.SeekRange;
		ai.disable = false;
		if (ai.seekRange == 0)
			ai.seekRange = def.GetAttacks()[0].MaxRange;
	}


	return unit;
}


void UnitSystem::PrepareUnitAI(EntityManager& em)
{
	for (auto& state : _aiStates)
	{
		state->thinkData.clear();
	}


	auto& entities = _aiComponents.GetEntities();
	auto& ai = _aiComponents.GetComponents();

	for (int i = 0; i < entities.size(); ++i)
	{
		if (ai[i].attackCooldown)
			--ai[i].attackCooldown;

		if (ai[i].disable) continue;

		UnitAIStateId stateId = ai[i].stateId;
		EntityId id = entities[i];

	

		_aiStates[(int)stateId]->thinkData.entities.push_back(id);

	}

	for (auto& state : _aiStates)
	{
		state->Start(0);
	}

	_aiUpdatesCompleted = 0;
}
bool UnitSystem::UpdateUnitAI(EntityManager& em)
{
	// Todo: have 2 states list, for enter state and for think to avid ifs

	int batch = 64;

	UnitAIState* state = _aiStates[_aiUpdatesCompleted];


	if (state->thinkFunc && !state->IsCompleted())
	{
		state->Advance(batch);
		state->thinkFunc(state->thinkData, em);

		return false;
	}

	++_aiUpdatesCompleted;

	return _aiUpdatesCompleted == _aiStates.size();
}
void UnitSystem::UpdateUnitStates(EntityManager& em)
{
	for (UnitState* state : _unitStates)
	{
		state->EnterState.clear();
		state->ExitState.clear();
	}

	auto& entities = _unitStateComponents.GetEntities();
	auto& state = _unitStateComponents.GetComponents();

	for (int i = 0; i < entities.size(); ++i)
	{
		EntityId id = entities[i];
		if (state[i].newState)
		{
			UnitStateId newState = state[i].stateId;
			UnitStateId currentState = state[i].currentStateId;

			state[i].newState = false;
			_unitStates[(int)currentState]->ExitState.entities.push_back(id);
			_unitStates[(int)currentState]->ExitState.otherState.push_back(newState);
			_unitStates[(int)newState]->EnterState.entities.push_back(id);
			_unitStates[(int)newState]->EnterState.otherState.push_back(currentState);
			state[i].currentStateId = newState;
		}
	}

	for (auto& state : _unitStates)
	{
		state->ExitState.Start(0);
		if (state->exitStateFunc && state->ExitState.size())
		{
			state->ExitState.end = state->ExitState.size();
			state->exitStateFunc(state->ExitState, em);
		}

		state->EnterState.Start(0);
		if (state->enterStateFunc && state->EnterState.size())
		{
			state->EnterState.end = state->EnterState.size();
			state->enterStateFunc(state->EnterState, em);
		}
	}


}

void UnitSystem::ProcessUnitEvents(EntityManager& em)
{
	for (UnitComponent& unit : _unitComponents.GetComponents())
	{
		if (unit.shield < unit.maxShield)
		{
			uint8_t old = unit.shieldRegen;
			unit.shieldRegen += ShieldRegen;

			if (unit.shieldRegen < old)
			{
				unit.shield.value += 2;
			}
		}

		unit.attackedBy = Entity::None;
	}

	for (EntityId id : _unitAttackEvents)
	{
		UnitComponent& unit = GetComponent(id);
		UnitAIComponent& ai = GetAIComponent(id);

		if (ai.targetEntity == Entity::None || !em.HasEntity(ai.targetEntity) || !em.UnitSystem.IsUnit(ai.targetEntity)) continue;

		auto& attack = unit.def->GetAttacks()[ai.attackId];
		auto sound = attack.GetWeapon()->GetSpawnSound();

		if (sound)
		{
			EntityUtil::GetManager().SoundSystem.PlayWorldSound(*sound, EntityUtil::GetManager().GetPosition(id), 0);
		}


		UnitComponent& target = GetComponent(ai.targetEntity);

		if (target.IsDead())
		{
			ai.targetEntity = Entity::None;
			continue;
		}

		auto damage = unit.damage[ai.attackId];

		if (target.shield.value > 0)
		{
			short shieldDamage = std::min(target.shield.value, damage.value);
			target.shield.value -= shieldDamage;
			damage.value -= shieldDamage;

			uint8_t orientation = EntityUtil::GetOrientationToPosition(ai.targetEntity, em.GetPosition(id));

			Vector2Int16 pos = em.GetPosition(ai.targetEntity);
			auto dst = Vector2(em.GetPosition(id)) - Vector2(pos);
			dst.Normalize();
			dst *= 8;
			pos += Vector2Int16(dst);

			EntityId id = EntityUtil::SpawnSprite(*GameDatabase::instance->ProtossShieldSprite, pos, orientation);

			em.DrawSystem.GetComponent(id).depth = em.DrawSystem.GetComponent(ai.targetEntity).depth + 1;
		}

		if (damage.value > 0)
			damage.value = std::max((short)1, (damage - target.armor).value);

		if (damage.value > 0)
			target.health -= damage;

		ai.attackCooldown = attack.Cooldown;

		target.attackedBy = id;

		if (target.IsDead())
		{
			_unitDieEvents.push_back(ai.targetEntity);
			ai.targetEntity = Entity::None;
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
				em.DrawSystem.GetComponent(death).depth - em.DrawSystem.GetComponent(id).depth;
				em.DrawSystem.GetComponent(death).color = em.DrawSystem.GetComponent(id).color;
				em.AnimationSystem.NewComponent(death);
				em.SetPosition(death, em.GetPosition(id));
				em.SetOrientation(death, em.GetOrientation(id));
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

