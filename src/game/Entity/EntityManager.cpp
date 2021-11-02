#include "EntityManager.h"

#include "../Profiler.h"
#include "../Util.h"
#include "../Debug.h"
#include "EntityUtil.h"

#include <algorithm>


class EntityManagerUpdateCrt :public CoroutineImpl {

	EntityManager* _em;
	SectionProfiler _p = SectionProfiler("none");
	SectionProfiler _updateProfiler = SectionProfiler("Update");

	void BeignProfile(const char* name)
	{
		_p = SectionProfiler(name);
		_p.statMinThreshold = 0;
	}
	void EndProfile()
	{
		_p.Submit();
	}
public:
	EntityManagerUpdateCrt(EntityManager* em) : _em(em) {}

	CRT_START()
	{

		_updateProfiler = SectionProfiler("UpdateCrt");

		_em->PlayerSystem.ResetNewEvents();

		BeignProfile("UpdateVision");

		while (!_em->PlayerSystem.UpdateNextPlayerVision())
		{
			CRT_YIELD();
		}

		EndProfile();

		BeignProfile("UpdateUnitAI");
		_em->UnitSystem.PrepareUnitAI(*_em);

		while(!_em->UnitSystem.UpdateUnitAI(*_em)) {
			CRT_YIELD();
		}
			

		EndProfile();

		CRT_YIELD();

		_em->PlayerSystem.FinishCollectingEvents();

		_em->UnitSystem.ProcessUnitEvents(*_em);

		BeignProfile("RunAnimations");
		_em->AnimationSystem.RunAnimations(*_em);
		EndProfile();

		_em->ApplyEntityChanges();

		_em->PlayerSystem.UpdatePlayers(*_em);

		BeignProfile("UpdateColliders");
		_em->KinematicSystem.UpdateColliders(*_em);
		EndProfile();

		BeignProfile("UpdatePositions");
		_em->DrawSystem.UpdatePositions(*_em);
		EndProfile();

		BeignProfile("UpdateVisibleObjects");
		_em->MapSystem.UpdateVisibleObjects(*_em);
		EndProfile();

		++_em->logicalFrame;

		_updateProfiler.Submit();

	}
	CRT_END();
};

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

Coroutine EntityManager::NewUpdateCoroutine()
{
	return Coroutine(new EntityManagerUpdateCrt(this));
}
void EntityManager::UpdateAudio(const Camera& camera)
{
	SoundSystem.UpdateSounds(*this, camera);
}
void EntityManager::Draw(const Camera& camera)
{
	SectionProfiler p("Draw");

	MapSystem.DrawOffscreenData(*this);

	MapSystem.DrawMap(camera);

	DrawSystem.Draw(*this, camera);

	MapSystem.DrawFogOfWar(*this, camera);
	//if (DrawBoundingBoxes)
	KinematicSystem.DrawColliders(camera);
	//if (DrawGrid)

	frameCounter++;

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



