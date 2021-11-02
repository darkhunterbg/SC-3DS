#include "EntityManager.h"

#include "../Profiler.h"
#include "../Util.h"
#include "../Debug.h"
#include "EntityUtil.h"

#include <algorithm>

EntityManager::EntityManager()
{
	memset(_orientations.data(), 0, _orientations.size());
	EntityUtil::emInstance = this;

	_systems.push_back(&DrawSystem);
	_systems.push_back(&AnimationSystem);
	_systems.push_back(&PlayerSystem);
	_systems.push_back(&MapSystem);
	_systems.push_back(&UnitSystem);
	_systems.push_back(&KinematicSystem);
	_systems.push_back(&SoundSystem);
}
EntityManager::~EntityManager()
{
}

void EntityManager::Init(Vector2Int16 mapSize)
{
	PlayerSystem.SetSize(mapSize);
	MapSystem.SetSize(mapSize);
}

void EntityManager::DeleteEntity(EntityId id)
{
	toDelete.push_back(id);
}

void EntityManager::DeleteEntitiesSorted(std::vector<EntityId>& e)
{
	int max = e.size();

	if (e.size() != max)
		std::sort(e.begin(), e.end());

	toDelete.insert(toDelete.end(), e.begin(), e.end());
}
void EntityManager::DeleteEntities(const std::vector<EntityId>& e, bool sorted)
{
	scratch.clear();
	scratch.insert(scratch.begin(), e.begin(), e.end());

	if (!sorted)
		std::sort(scratch.begin(), scratch.end());

	DeleteEntitiesSorted(scratch);
}

void EntityManager::ClearEntities()
{
	toDelete.clear();
	toDelete.insert(toDelete.end(), entities.GetEntities().begin(), entities.GetEntities().end());

	for (auto& system : _systems)
		system->DeleteEntities(toDelete);
	
	if (OnEntitiesDeleted != nullptr)
		OnEntitiesDeleted(toDelete);

	toDelete.clear();
	entities.ClearEntities();
}

void EntityManager::ApplyEntityChanges()
{
	if (toDelete.size() > 0)
	{
		std::sort(toDelete.begin(), toDelete.end());

		for (auto& system : _systems)
			system->DeleteEntities(toDelete);

		if (OnEntitiesDeleted != nullptr)
			OnEntitiesDeleted(toDelete);

		entities.DeleteSortedEntities(toDelete);
		toDelete.clear();
	}
}

// Updates 12 per second (60 fps) 
void EntityManager::Update0()
{
	PlayerSystem.ResetNewEvents();

	PlayerSystem.UpdateNextPlayerVision();
}
// Updates 24 per second (60 fps) 
void EntityManager::Update1()
{
	PlayerSystem.ResetNewEvents();

	PlayerSystem.UpdateNextPlayerVision();

	UnitSystem.UpdateUnitAI(*this);

}
// Update 24 per second (60 fps) 
void EntityManager::Update2()
{
	PlayerSystem.FinishCollectingEvents();

	UnitSystem.ProcessUnitEvents(*this);

	AnimationSystem.RunAnimations(*this);

	ApplyEntityChanges();

	PlayerSystem.UpdatePlayers(*this);

	KinematicSystem.UpdateColliders(*this);

	DrawSystem.UpdatePositions(*this);

	MapSystem.UpdateVisibleObjects(*this);

	++logicalFrame;
}

// Draws 12 per second (60 fps) 
void EntityManager::Draw0(const Camera& camera)
{
	MapSystem.DrawOffscreenData(*this);
	SoundSystem.UpdateSounds(*this, camera);
}
// Draws 24 per second (60 fps) 
void EntityManager::Draw1(const Camera& camera)
{

}
// Draws 24 per second (60 fps) 
void EntityManager::Draw2(const Camera& camera)
{

}

void EntityManager::FrameUpdate(const Camera& camera)
{

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

void EntityManager::FullUpdate()
{
	Update0();
	Update1();
	Update2();
}

size_t EntityManager::GetMemoryUsage()
{
	size_t size = +entities.GetMemoryUsage();
	for (auto system : _systems)
		size += system->ReportMemoryUsage();

	size += scratch.capacity() * sizeof(EntityId);
	size += toDelete.capacity() * sizeof(EntityId);
	size += sizeof(_positions);
	size + sizeof(_orientations);

	return size;
}

void EntityManager::Draw(const Camera& camera)
{

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

		MapSystem.DrawMap(camera);

		DrawSystem.Draw(*this, camera);

		MapSystem.DrawFogOfWar(*this, camera);
		//if (DrawBoundingBoxes)


		KinematicSystem.DrawColliders(camera);

		//if (DrawGrid)

		frameCounter++;
		updateId = (frameCounter % 5);
		if (updateId > 2)
			updateId = updateId % 3 + 1;
	}
}

