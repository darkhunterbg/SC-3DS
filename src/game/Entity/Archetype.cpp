#include "Archetype.h"
#include "Debug.h"
#include "../Profiler.h"

#include <algorithm>
#include <cstring>

EntityArchetype::EntityArchetype(const char* name) : name(name)
{
	memset(hasEntity.data(), false, Entity::MaxEntities * sizeof(bool));
}

void EntityArchetype::AddEntity(EntityId id)
{
	if (HasEntity(id))
		EXCEPTION("Entity %i already added to archetype %s!", id, name);

	hasEntity[Entity::ToIndex(id)] = true;
	newEntities.push_back(id);
	entities.AddEntity(id);
}

void EntityArchetype::AddSortedEntities(const std::vector<EntityId>& add) {
	for (EntityId id : add)
		if (HasEntity(id)) {
			EXCEPTION("Entity %i is already part of archetype %s!", id, name);
		}
		else
			hasEntity[Entity::ToIndex(id)] = true;

	newEntities.insert(newEntities.end(), add.begin(), add.end());
	entities.AddSortedEntities(add);
}
void EntityArchetype::AddEntities(std::vector<EntityId>& add, bool sorted) {
	if (!sorted)
		std::sort(add.begin(), add.end());

	AddSortedEntities(add);
}

void EntityArchetype::RemoveEntity(EntityId id)
{
	if (!HasEntity(id))
		EXCEPTION("Entity %i is not part of archetype %s!", id, name);

	hasEntity[Entity::ToIndex(id)] = false;
	removedEntities.push_back(id);

	if (!entities.RemoveEntity(id))
		EXCEPTION("Deleting entity %id was not found in archetype %s!", id, name);
}
int EntityArchetype::RemoveSortedEntities(std::vector<EntityId>& del) {
	scratch.clear();

	for (EntityId id : del)
		if (!HasEntity(id))
			continue;
		else {
			scratch.push_back(id);
			hasEntity[Entity::ToIndex(id)] = false;
		}

	if (scratch.size() == 0)
		return 0;

	removedEntities.insert(removedEntities.end(), scratch.begin(), scratch.end());

	entities.RemoveSortedEntities(scratch);

	return scratch.size();
}
int EntityArchetype::RemoveEntities(std::vector<EntityId>& del, bool sorted) {
	if (!sorted)
		std::sort(del.begin(), del.end());

	return RemoveSortedEntities(del);
}

void EntityArchetype::ClearEntities()
{
	entities.clear();
	newEntities.clear();
	removedEntities.clear();
	memset(hasEntity.data(), false, Entity::MaxEntities * sizeof(bool));
}
void EntityArchetype::CommitChanges()
{
	newEntities.clear();
	removedEntities.clear();
}
