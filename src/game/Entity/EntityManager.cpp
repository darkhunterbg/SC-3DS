#include "EntityManager.h"

#include "../Profiler.h"
#include "../Util.h"
#include "../Debug.h"
#include "EntityUtil.h"

#include <algorithm>

EntityManager::EntityManager() {
	memset(_orientations.data(), 0, _orientations.size());
	EntityUtil::emInstance = this;
}
EntityManager::~EntityManager() {
}

void EntityManager::Init(Vector2Int16 mapSize)
{
}

void EntityManager::DeleteEntity(EntityId id) {
	toDelete.push_back(id);
}

void EntityManager::DeleteEntitiesSorted(std::vector<EntityId>& e) {
	int max = e.size();

	if (e.size() != max)
		std::sort(e.begin(), e.end());

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
}


void EntityManager::CollectEntityChanges() {
	changedData.clear();
	for (EntityId id : entities) {
		int i = Entity::ToIndex(id);
	}
}
void EntityManager::ApplyEntityChanges() {
	for (EntityId id : changedData.entity) {
		int i = Entity::ToIndex(id);
	}

	if (toDelete.size() > 0)
	{
		std::sort(toDelete.begin(), toDelete.end());
		entities.DeleteSortedEntities(toDelete);
		toDelete.clear();
	}
}
void EntityManager::UpdateChildren() {
}

// Updates 12 per second (60 fps) 
void EntityManager::Update0() {


	//soundSystem.UpdateEntityAudio(camera, *this);
}
// Updates 24 per second (60 fps) 
void EntityManager::Update1() {


}
// Update 24 per second (60 fps) 
void EntityManager::Update2() {
	AnimationSystem.RunAnimations(*this);

	UpdateChildren();

	CollectEntityChanges();

	ApplyEntityChanges();

	++logicalFrame;
}

// Draws 12 per second (60 fps) 
void EntityManager::Draw0(const Camera& camera) {
}
// Draws 24 per second (60 fps) 
void EntityManager::Draw1(const Camera& camera) {

}
// Draws 24 per second (60 fps) 
void EntityManager::Draw2(const Camera& camera) {}

void EntityManager::FrameUpdate(const Camera& camera) {

	ready = true;

	switch (updateId)
	{
	case 0: {
		SectionProfiler p("Update0");
		Update0();

		break;
	}
	case 1: {
		SectionProfiler p("Update1");
		Update1();
		break;
	}
	case 2: {
		SectionProfiler p("Update2");
		Update2();
		break;
	}
	default:
		EXCEPTION("Invalid UpdateId %i", updateId);
	}

	//Util::RealTimeStat("Entities", entities.size());
}

void EntityManager::FullUpdate() {

	Update0();
	Update1();
	Update2();
}

void EntityManager::Draw(const Camera& camera) {

	SectionProfiler p("Draw");

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


		DrawSystem.Draw(*this, camera);

		//if (DrawBoundingBoxes)

		//if (DrawColliders)

		//if (DrawGrid)

		frameCounter++;
		updateId = (frameCounter % 5);
		if (updateId > 2)
			updateId = updateId % 3 + 1;
	}
}

