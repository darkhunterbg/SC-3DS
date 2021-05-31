#include "UnitSystem.h"
#include "../Profiler.h"
#include "EntityManager.h"
#include "EntityUtil.h"
#include "../Game.h"
#include "../Engine/JobSystem.h"

#include "../Data/GraphicsDatabase.h"

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

		FlagsComponent& flags = em.FlagComponents.GetComponent(id);

		if (flags.test(ComponentFlags::UnitAIPaused))
			continue;

		UnitAIState state = em.UnitArchetype.AIStateComponents.GetComponent(id);

		if (flags.test(ComponentFlags::UnitAIStateChanged)) {
			aiEnterStateData[(int)state].entities.push_back(id);
			flags.clear(ComponentFlags::UnitAIStateChanged);
		}

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
		if (data.size())
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

		auto& healthComponent = em.UnitArchetype.HealthComponents.GetComponent(id);

		auto& unitDataComponent = em.UnitArchetype.DataComponents.GetComponent(id);
		auto& unit = em.UnitArchetype.UnitComponents.GetComponent(id);
		if (unitDataComponent.isBuilding) {


			// ========== DIRTY ====================

			//if (healthComponent.current < (healthComponent.max << 4) / 5) {
			//	if (unit.fires[0] == Entity::None) {
			//		auto e2 = em.NewEntity();
			//		em.OldPositionComponents.NewComponent(e2, em.PositionComponents.GetComponent(id));
			//		em.FlagComponents.NewComponent(e2);
			//		em.RenderArchetype.RenderComponents.GetComponent(e2).depth = 1;
			//		em.RenderArchetype.Archetype.AddEntity(e2);
			//		em.AnimationArchetype.Archetype.AddEntity(e2);

			//		EntityUtil::PlayAnimation(e2, GraphicsDatabase::FireDamage.Left[0]);
			//		EntityUtil::SetRenderFromAnimationClip(e2, GraphicsDatabase::FireDamage.Left[0],0);

			//		em.ParentArchetype.Archetype.AddEntity(id);
			//		em.ParentArchetype.ChildComponents.NewComponent(id).AddChild(e2);

			//		em.UnitArchetype.UnitComponents.GetComponent(id).fires[0] = e2;

			//	
			//	}
			//}

			// =====================================

			if (healthComponent.current < healthComponent.max / 3) {

				if (unitDataComponent.internalTimer == 0)
				{
					healthComponent.ReduceUnmitigated(1);
					unitDataComponent.ResetFireTimer();
				}
				--unitDataComponent.internalTimer;
			}
		}


		if (em.UnitArchetype.HealthComponents.GetComponent(id).IsDead()) {
			em.UnitArchetype.StateComponents.GetComponent(id) = UnitState::Death;
			em.FlagComponents.GetComponent(id).set(ComponentFlags::UnitStateChanged);
		}
	}

}

struct Spawn {
	const UnitDef* def;
	Vector2Int16 pos;
	PlayerId owner;
	uint8_t orientation;
};

static std::vector<Spawn> spawn;
static std::vector<Vector2Int16> testPositions;

void UnitSystem::UpdateBuilding(EntityManager& em)
{
	spawn.clear();

	for (const auto& cmd : unitBuildUpdate) {

		EntityId id = cmd.id;
		const UnitDef& unit = *cmd.def;

		auto& data = em.UnitArchetype.DataComponents.GetComponent(id);

		PlayerId playerId = em.UnitArchetype.OwnerComponents.GetComponent(id);

		if (data.build) {
			if (!data.IsQueueFull()) {

				if (em.GetPlayerSystem().HasEnoughMinerals(playerId, unit.MineralCost)) {
					data.EnqueueProduce(unit);
					em.GetPlayerSystem().AddMinerals(playerId, -unit.MineralCost);
				}
				else {
					em.GetPlayerSystem().NewEvent(playerId, PlayerEventType::NotEnoughMinerals, id);
				}
			}
		}
		else {
			if ( em.GetPlayerSystem().HasEnoughFreeSupply(playerId, unit.UseSupplyDoubled)) {

				if (cmd.paid || em.GetPlayerSystem().HasEnoughMinerals(playerId, unit.MineralCost)) {
					if (data.IsQueueEmpty())
						data.EnqueueProduce(unit);
					data.build = true;

					if (!cmd.paid)
						em.GetPlayerSystem().AddMinerals(playerId, -unit.MineralCost);

					em.GetPlayerSystem().ReserveSupply(playerId, unit.UseSupplyDoubled);
				}
				else {
					em.GetPlayerSystem().NewEvent(playerId, PlayerEventType::NotEnoughMinerals, id);
				}
			}
			else {
				em.GetPlayerSystem().NewEvent(playerId, PlayerEventType::NotEnoughSupply, id);
			}
		}

	}

	unitBuildUpdate.clear();

	for (EntityId id : em.UnitArchetype.Archetype.GetEntities()) {

		UnitDataComponent& data = em.UnitArchetype.DataComponents.GetComponent(id);
		PlayerId owner = em.UnitArchetype.OwnerComponents.GetComponent(id);

	

		if (!data.IsQueueEmpty()) {
			const UnitDef* def = data.productionQueue[0];

			auto& state = em.UnitArchetype.StateComponents.GetComponent(id);

			if (state != UnitState::Producing) {
				state = UnitState::Producing;
				em.FlagComponents.GetComponent(id).set(ComponentFlags::UnitStateChanged);
			}

			if (!data.build) {
				if (!data.IsQueueEmpty()) {
					const UnitDef* def = data.productionQueue[0];
					if (em.GetPlayerSystem().HasEnoughFreeSupply(owner, def->UseSupplyDoubled))
						EnqueueBuildUpdateCheck(id, *data.productionQueue[0], true);
				}
				continue;
			}


			bool completed = data.TickQueue(1);
			if (completed) {
				const UnitDef* def = data.Dequeue();

				if (!data.IsQueueEmpty()) {
					EnqueueBuildUpdateCheck(id, *data.productionQueue[0], true);
				}
				else {
					state = UnitState::Idle;
					em.FlagComponents.GetComponent(id).set(ComponentFlags::UnitStateChanged);
				}

				Rectangle16 iterate = em.CollisionArchetype.ColliderComponents.GetComponent(id)
					.collider;

				iterate.position += em.PositionComponents.GetComponent(id);

				Vector2Int16 bound = def->Graphics->Collider.size;

				iterate.position = iterate.position - bound / 2;
				iterate.size = iterate.size + bound;

				testPositions.clear();

				for (int x = bound.x; x < iterate.size.x; x += bound.x + 1) {
					Vector2Int16 pos = Vector2Int16(x, iterate.size.y) + iterate.position;
					testPositions.push_back(pos);
				}
				for (int y = iterate.size.y; y > bound.y; y -= bound.y + 3) {
					Vector2Int16 pos = Vector2Int16(iterate.size.x, y) + iterate.position;
					testPositions.push_back(pos);
				}
				for (int x = iterate.size.x; x > 0; x -= bound.x + 1) {
					Vector2Int16 pos = Vector2Int16(x, -1) + iterate.position;
					testPositions.push_back(pos);
				}
				for (int y = -1; y < iterate.size.y; y += bound.y + 3) {
					Vector2Int16 pos = Vector2Int16(-1, y) + iterate.position;
					testPositions.push_back(pos);
				}

				for (Vector2Int16 pos : testPositions)
				{
					Rectangle16 dstCollider = def->Graphics->Collider;
					dstCollider.SetCenter(pos);

					if (!em.GetKinematicSystem().CollidesWithAny(dstCollider, Entity::None))
					{

						uint8_t orientation = EntityUtil::GetOrientationToPosition(id, pos);
						spawn.push_back({ def, pos, owner , orientation });
						break;
					}
				}
			}
		}

	}

	PlayerSystem& ps = em.GetPlayerSystem();

	for (const Spawn& s : spawn) {
		EntityId e = UnitEntityUtil::NewUnit(*s.def, s.owner, s.pos);
		em.OrientationComponents.GetComponent(e) = s.orientation;
		ps.NewEvent(s.owner, PlayerEventType::NewUnit, e);
		ps.ReserveSupply(s.owner, -s.def->UseSupplyDoubled);
	}
}

void UnitSystem::DequeueItem(EntityId id, int itemId, EntityManager& em)
{
	UnitDataComponent& data = em.UnitArchetype.DataComponents.GetComponent(id);
	PlayerId owner = em.UnitArchetype.OwnerComponents.GetComponent(id);


	bool restoreSupply = data.build && itemId == 0;
	const UnitDef* def = data.RemoveFromQueue(itemId);

	if (def == nullptr)
		return;

	if (restoreSupply)
		em.GetPlayerSystem().ReserveSupply(owner, -def->UseSupplyDoubled);
	em.GetPlayerSystem().AddMinerals(owner, -def->MineralCost);

	if (!data.IsQueueEmpty()) {
		if (itemId == 0) {
			def = data.productionQueue[0];

			if (em.GetPlayerSystem().HasEnoughFreeSupply(owner, def->UseSupplyDoubled)) {
				data.build = true;
				em.GetPlayerSystem().ReserveSupply(owner, def->UseSupplyDoubled);
			}
			else {
				data.build = false;
				em.GetPlayerSystem().NewEvent(owner, PlayerEventType::NotEnoughSupply, id);
			}
		}
	}
	else {
		auto& state = em.UnitArchetype.StateComponents.GetComponent(id);
		state = UnitState::Idle;
		em.FlagComponents.GetComponent(id).set(ComponentFlags::UnitStateChanged);
	}
}
