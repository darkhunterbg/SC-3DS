#include "EntityManager.h"
#include "RenderSystem.h"
#include "EntityUtil.h"

#include "../Profiler.h"
#include "../Util.h"

#include <algorithm>


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
	archetypes.push_back(&ParentArchetype.Archetype);
	archetypes.push_back(&HiddenArchetype.Archetype);
	archetypes.push_back(&UnitArchetype.HiddenArchetype.Archetype);
	archetypes.push_back(&MapObjectArchetype.Archetype);
	archetypes.push_back(&UnitArchetype.FowVisibleArchetype.Archetype);
	archetypes.push_back(&AnimationArchetype.OrientationArchetype.Archetype);
	archetypes.push_back(&UnitArchetype.AnimationArchetype.OrientationArchetype.Archetype);
	archetypes.push_back(&SoundArchetype.Archetype);

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
	playerSystem.SetSize(mapSize);
}

void EntityManager::DeleteEntity(EntityId id) {
	if (ParentArchetype.Archetype.HasEntity(id)) {
		const auto& cmp = ParentArchetype.ChildComponents.GetComponent(id);
		for (int i = 0; i < cmp.childCount; ++i) {
			DeleteEntity(cmp.children[id]);
		}
	}

	toDelete.push_back(id);
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
		arch->RemoveSortedEntities(e);
	}


	toDelete.insert(toDelete.end(), e.begin(), e.end());
}
void EntityManager::DeleteEntities(const std::vector<EntityId>& e, bool sorted) {

	scratch.clear();
	scratch.insert(scratch.begin(), e.begin(), e.end());

	if (!sorted)
		std::sort(scratch.begin(), scratch.end());

	DeleteEntitiesSorted(scratch);

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
			changedData.oldPosition.push_back(OldPositionComponents[i]);
		}
	}
}
void EntityManager::ApplyEntityChanges() {
	for (EntityId id : changedData.entity) {
		int i = Entity::ToIndex(id);
		FlagComponents[i].clear(ComponentFlags::PositionChanged);
		OldPositionComponents[i] = PositionComponents[i];
	}

	for (auto archetype : archetypes)
		archetype->CommitChanges();

	if (toDelete.size() > 0)
	{
		std::sort(toDelete.begin(), toDelete.end());
		entities.DeleteSortedEntities(toDelete);
		toDelete.clear();
	}
}
void EntityManager::UpdateChildren() {
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

		if (FlagComponents.GetComponent(id).test(ComponentFlags::OrientationChanged)) {
			auto& childComp = ParentArchetype.ChildComponents.GetComponent(id);

			const uint8_t& orientation = OrientationComponents.GetComponent(id);

			for (int i = 0; i < childComp.childCount; ++i) {
				EntityId child = childComp.children[i];

				OrientationComponents.GetComponent(child) = orientation;
				FlagComponents.GetComponent(child).set(ComponentFlags::OrientationChanged);
			}
		}
	}
}

// Updates 12 per second (60 fps) 
void EntityManager::Update0(const Camera& camera) {

	playerSystem.UpdateNextPlayerVision();

	soundSystem.UpdateEntityAudio(camera, *this);

}
// Updates 24 per second (60 fps) 
void EntityManager::Update1(const Camera& camera) {

	playerSystem.UpdateNextPlayerVision();

	timingSystem.UpdateTimers(*this);

	//navigationSystem.UpdateNavGrid(*this);
	unitSystem.UpdateUnitCooldowns(*this);

	navigationSystem.UpdateNavigation(*this);

	navigationSystem.ApplyUnitNavigaion(*this);

	unitSystem.UnitAIUpdate(*this);

	animationSystem.TickAnimations(*this);
}
// Update 24 per second (60 fps) 
void EntityManager::Update2(const Camera& camera) {
	timingSystem.ApplyTimerActions(*this);

	unitSystem.UpdateUnitStats(*this);

	unitSystem.ApplyUnitState(*this);

	kinematicSystem.MoveEntities(*this);

	UpdateChildren();

	animationSystem.UpdateAnimationsForOrientation(*this);

	mapSystem.UpdateVisibleEntities(*this);

	animationSystem.UpdateAnimations(*this);

	CollectEntityChanges();

	playerSystem.UpdatePlayerUnits(*this);
	mapSystem.UpdateMap(*this, changedData);
	renderSystem.UpdatePositions(*this, changedData);
	kinematicSystem.UpdateCollidersPosition(*this, changedData);
	kinematicSystem.ApplyCollidersChange(*this);

	ApplyEntityChanges();
}

// Draws 12 per second (60 fps) 
void EntityManager::Draw0(const Camera& camera) {
	mapSystem.RedrawMinimap(*this);
}
// Draws 24 per second (60 fps) 
void EntityManager::Draw1(const Camera& camera) {

}
// Draws 24 per second (60 fps) 
void EntityManager::Draw2(const Camera& camera) {

}

static int updates[3] = { 0,0,0 };

int last = -1;

void EntityManager::FrameUpdate(const Camera& camera) {

	ready = true;
	//FullUpdate(camera);

	if (updateId == last)
		EXCEPTION("TRIED TO UPDATE TWICE");

	switch (updateId)
	{
	case 0: {
		SectionProfiler p("Update0");

		Update0(camera);

		updates[0]++;
		break;
	}
	case 1: {
		SectionProfiler p("Update1");
		Update1(camera);

		updates[1]++;
		break;
	}
	case 2: {
		SectionProfiler p("Update2");
		Update2(camera);


		updates[2]++;
		break;
	}
	default:
		EXCEPTION("Invalid UpdateId %i", updateId);
	}


	last = updateId;

	//Util::RealTimeStat("Entities", entities.size());
}

void EntityManager::FullUpdate(const Camera& camera) {
	Update0(camera);
	Update1(camera);
	Update2(camera);
}

void EntityManager::Draw(const Camera& camera) {

	if (ready)
	{
		switch (updateId)
		{
		case 0:
			Draw0(camera);  break;
		case 1:
			Draw1(camera);  break;
		case 2:
			Draw2(camera);  break;
		default:
			EXCEPTION("Invalid UpdateId %i", updateId);
		}

		mapSystem.DrawMap(camera);
		renderSystem.Draw(camera, *this);

		if (DrawBoundingBoxes)
			renderSystem.DrawBoundingBoxes(camera, *this);

		if (DrawColliders)
			kinematicSystem.DrawColliders(camera);

		mapSystem.DrawFogOfWar(camera);
		if (DrawGrid)
			mapSystem.DrawGrid(camera);



		soundSystem.UpdateChatRequest(*this);

		frameCounter++;
		updateId = (frameCounter % 5);
		if (updateId > 2)
			updateId = updateId % 3 + 1;
	}
}

