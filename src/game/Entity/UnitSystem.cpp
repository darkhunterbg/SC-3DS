#include "UnitSystem.h"
#include "../Profiler.h"
#include "EntityManager.h"
#include "EntityUtil.h"
#include "../Game.h"
#include "../Job.h"

// =============================== Unit System =================================

UnitSystem::UnitSystem()
{
	for (int i = 0; i < UnitAIStateMachine::States.size(); ++i) {
		aiThinkData.push_back(UnitAIThinkData());
		aiEnterStateData.push_back(UnitAIEnterStateData());
	}

	for (int i = 0; i < UnitStateMachine::States.size(); ++i) {
		exitStateData.push_back(UnitStateMachineChangeData());
		enterStateData.push_back(UnitStateMachineChangeData());
	}
}

void UnitSystem::UnitAIUpdate(EntityManager& em) {
	for (auto& data : aiThinkData) {
		data.clear();
	}
	for (auto& data : aiEnterStateData) {
		data.clear();
	}

	for (EntityId id : em.UnitArchetype.Archetype.GetEntities()) {

		if (em.FlagComponents.GetComponent(id).test(ComponentFlags::UnitAIPaused))
			continue;

		UnitAIState state = em.UnitArchetype.AIStateComponents.GetComponent(id);

		if (em.FlagComponents.GetComponent(id).test(ComponentFlags::UnitAIStateChanged))
			aiEnterStateData[(int)state].entities.push_back(id);

		const auto& stateData = em.UnitArchetype.AIStateDataComponents.GetComponent(id);
		const auto& position = em.PositionComponents.GetComponent(id);
		PlayerId owner = em.UnitArchetype.OwnerComponents.GetComponent(id);


		auto& thinkData = aiThinkData[(int)state];

		thinkData.entities.push_back(id);
		thinkData.stateData.push_back(stateData);
		thinkData.position.push_back(position);
		thinkData.owner.push_back(owner);
	}



	for (int i = 0; i < UnitAIStateMachine::States.size(); ++i) {
		auto& state = *UnitAIStateMachine::States[i];
		auto& enterStateData = aiEnterStateData[i];
		auto& thinkData = aiThinkData[i];

		if (enterStateData.size())
			state.EnterState(enterStateData, em);

		if (thinkData.size())
			state.Think(thinkData, em);
	}

}

void UnitSystem::UpdateUnitCooldowns(EntityManager& em)
{
	for (EntityId id : em.UnitArchetype.Archetype.GetEntities()) {
		auto& weapon = em.UnitArchetype.WeaponComponents.GetComponent(id);
		if (!weapon.IsReady())
			--weapon.remainingCooldown;
	}
}

void UnitSystem::ApplyUnitState(EntityManager& em) {
	for (auto& data : exitStateData)
		data.clear();
	for (auto& data : enterStateData)
		data.clear();

	for (EntityId id : em.UnitArchetype.Archetype.GetEntities()) {

		FlagsComponent& flags = em.FlagComponents.GetComponent(id);
		if (flags.test(ComponentFlags::UnitStateChanged)) {
			flags.clear(ComponentFlags::UnitStateChanged);

			UnitState& prevState = em.UnitArchetype.PrevStateComponents.GetComponent(id);
			UnitState nextState = em.UnitArchetype.StateComponents.GetComponent(id);

			exitStateData[(int)prevState].entities.push_back(id);
			enterStateData[(int)nextState].entities.push_back(id);

			prevState = nextState;
		}
	}

	for (int i = 0; i < UnitStateMachine::States.size(); ++i) {
		auto& state = *UnitStateMachine::States[i];
		auto& data = exitStateData[i];
		if(data.size())
			state.ExitState(data, em);
	}

	for (int i = 0; i < UnitStateMachine::States.size(); ++i) {
		auto& state = *UnitStateMachine::States[i];
		auto& data = enterStateData[i];
		if (data.size())
			state.EnterState(data, em);
	}
}

void UnitSystem::UpdateUnitStats(EntityManager& em)
{
	for (EntityId id : em.UnitArchetype.Archetype.GetEntities()) {

		if (em.UnitArchetype.HealthComponents.GetComponent(id).IsDead()) {
			em.UnitArchetype.StateComponents.GetComponent(id) = UnitState::Death;
			em.FlagComponents.GetComponent(id).set(ComponentFlags::UnitStateChanged);
		}
	}

}
