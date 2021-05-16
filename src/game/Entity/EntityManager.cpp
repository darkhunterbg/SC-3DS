#include "EntityManager.h"
#include "RenderSystem.h"
#include "EntityUtil.h"

#include "../Profiler.h"

#include <algorithm>

static std::vector<EntityId> scratch;

EntityManager::EntityManager() {

	archetypes.push_back(&RenderArchetype.Archetype);
	archetypes.push_back(&AnimationArchetype.Archetype);
	archetypes.push_back(&NavigationArchetype.Archetype);
	archetypes.push_back(&CollisionArchetype.Archetype);
	archetypes.push_back(&MovementArchetype.Archetype);
	archetypes.push_back(&UnitArchetype.Archetype);
	archetypes.push_back(&UnitArchetype.RenderArchetype.Archetype);
	archetypes.push_back(&UnitArchetype.AnimationArchetype.Archetype);
	archetypes.push_back(&TimingArchetype.Archetype);
	archetypes.push_back(&ParentArchetype.Archetype);;
	EntityUtil::emInstance = this;
	UnitEntityUtil::emInstance = this;
}
EntityManager::~EntityManager() {
	EntityUtil::emInstance = nullptr;
}

void EntityManager::Init(Vector2Int16 mapSize)
{
	navigationSystem.SetSize(mapSize);
	kinematicSystem.SetSize(mapSize);
	mapSystem.SetSize(mapSize);
}

void EntityManager::DeleteEntity(EntityId id) {
	if (ParentArchetype.Archetype.HasEntity(id)) {
		const auto& cmp = ParentArchetype.ChildComponents.GetComponent(id);
		for (int i = 0; i < cmp.childCount; ++i) {
			DeleteEntity(cmp.children[id]);
		}
	}

	entities.DeleteEntity(id);
	for (auto& arch : archetypes) {
		if (arch->HasEntity(id))
			arch->RemoveEntity(id);
	}

}


void EntityManager::DeleteEntitiesSorted(std::vector<EntityId>& e) {
	int max = e.size();

	for (int i = 0; i < max; ++i) {
		EntityId id = e[i];

		if (ParentArchetype.Archetype.HasEntity(id)) {
			const auto& cmp = ParentArchetype.ChildComponents.GetComponent(id);
			e.insert(e.end(), cmp.children.begin(), cmp.children.begin() + cmp.childCount);
		}
	}

	if (e.size() != max)
		std::sort(e.begin(), e.end());


	for (auto& arch : archetypes) {
		arch->RemoveEntities(e, true);
	}

	entities.DeleteSortedEntities(e);
}
void EntityManager::DeleteEntities(const std::vector<EntityId>& e, bool sorted) {

	scratch.clear();
	scratch.insert(scratch.begin(), e.begin(), e.end());

	if (!sorted)
		std::sort(scratch.begin(), scratch.end());

	DeleteEntitiesSorted(scratch);

}
void EntityManager::ClearEntityArchetypes(EntityId id) {
	for (auto& arch : archetypes) {
		if (arch->HasEntity(id))
			arch->RemoveEntity(id);
	}

	if (ParentArchetype.Archetype.HasEntity(id)) {
		const auto& cmp = ParentArchetype.ChildComponents.GetComponent(id);
		for (int i = 0; i < cmp.childCount; ++i) {
			DeleteEntity(cmp.children[id]);
		}
	}
}
void EntityManager::ClearEntitiesArchetypes(std::vector<EntityId>& e, bool sorted) {
	if (!sorted)
		std::sort(e.begin(), e.end());

	scratch.clear();

	for (EntityId id : e) {
		if (ParentArchetype.Archetype.HasEntity(id)) {
			const auto& cmp = ParentArchetype.ChildComponents.GetComponent(id);
			scratch.insert(scratch.end(), cmp.children.begin(), cmp.children.begin() + cmp.childCount);
		}
	}

	if (scratch.size())
	{
		std::sort(scratch.begin(), scratch.end());
		DeleteEntitiesSorted(scratch);
	}

	for (auto& arch : archetypes) {
		arch->RemoveEntitiesSorted(e);
	}
}
void EntityManager::ClearEntities()
{
	entities.ClearEntities();
	for (auto& arch : archetypes) {
		arch->ClearEntities();
	}
}

void EntityManager::CollectEntityChanges() {
	changedData.clear();
	for (EntityId id : entities) {
		int i = Entity::ToIndex(id);
		if (FlagComponents[i].test(ComponentFlags::PositionChanged)) {
			changedData.entity.push_back(id);
			changedData.position.push_back(PositionComponents[i]);
		}
	}
}
void EntityManager::ApplyEntityChanges() {
	for (EntityId id : changedData.entity) {
		int i = Entity::ToIndex(id);
		FlagComponents[i].clear(ComponentFlags::PositionChanged);
	}

	for (auto archetype : archetypes)
		archetype->CommitChanges();
}
void EntityManager::UpdateChildrenPosition() {
	for (EntityId id : ParentArchetype.Archetype.GetEntities()) {

		if (FlagComponents.GetComponent(id).test(ComponentFlags::PositionChanged)) {
			auto& childComp = ParentArchetype.ChildComponents.GetComponent(id);

			const Vector2Int16& pos = PositionComponents.GetComponent(id);

			for (int i = 0; i < childComp.childCount; ++i) {
				EntityId child = childComp.children[i];

				PositionComponents.GetComponent(child) = pos;
				FlagComponents.GetComponent(child).set(ComponentFlags::PositionChanged);
			}
		}
	}
}

void EntityManager::UpdateSecondaryEntities() {
	mapSystem.UpdateMap(*this);

	timingSystem.UpdateTimers(*this);

	timingSystem.ApplyTimerActions(*this);

	//navigationSystem.UpdateNavGrid(*this);

	navigationSystem.UpdateNavigation(*this);

	navigationSystem.ApplyUnitNavigaion(*this);

	animationSystem.SetUnitOrientationAnimations(*this);

	animationSystem.TickAnimations(*this);

	renderSystem.UpdatePositions(*this, changedData);
}

void EntityManager::UpdateEntities() {

	updated = true;

	playerSystem.UpdatePlayerUnits(*this);

	kinematicSystem.MoveEntities(*this);

	animationSystem.UpdateAnimations();

	UpdateChildrenPosition();

	CollectEntityChanges();

	renderSystem.UpdatePositions(*this, changedData);
	kinematicSystem.UpdateCollidersPosition(*this, changedData);
	kinematicSystem.ApplyCollidersChange(*this);

	ApplyEntityChanges();

}

void EntityManager::DrawEntites(const Camera& camera) {

	if (!updated)
		return;

	mapSystem.DrawMap(camera);

	renderSystem.Draw(camera, *this);

	if (DrawColliders)
		kinematicSystem.DrawColliders(camera);
}

void EntityManager::GoTo(EntityId e, Vector2Int16 pos) {

	FlagComponents.GetComponent(e).set(ComponentFlags::NavigationWork);
	NavigationArchetype.NavigationComponents.GetComponent(e).target = pos;
	NavigationArchetype.NavigationComponents.GetComponent(e).targetHeading = 255;
}
