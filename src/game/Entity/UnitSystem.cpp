#include "UnitSystem.h"
#include "../Profiler.h"
#include "EntityManager.h"
#include "EntityUtil.h"
#include "../Game.h"
#include "../Job.h"


// =============================== Unit System =================================


void UnitSystem::ApplyUnitState(EntityManager& em) {
	SectionProfiler p("ApplyUnitState");

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
