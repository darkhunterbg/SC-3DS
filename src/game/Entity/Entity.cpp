#include "Entity.h"
#include "Debug.h"
#include <algorithm>
#include <cstring>


EntityCollection::EntityCollection(unsigned reserve)
{
	if (reserve)
		sortedEntities.reserve(reserve);
}


void EntityCollection::reserve(unsigned reserve) {
	sortedEntities.reserve(reserve);
}

void EntityCollection::clear()
{
	sortedEntities.clear();
}

void EntityCollection::AddSortedEntities(const std::vector<EntityId>& entities, int start) {

	if (!entities.size())
		return;


	int insertAt = sortedEntities.size();
	EntityId id = *(entities.cbegin() + start);
	auto sortEnd = sortedEntities.size();
	for (int i = 0; i < sortEnd; ++i)
	{
		if (sortedEntities[i] > id) {
			insertAt = i;
			break;
		}
	}

	sortedEntities.insert(sortedEntities.cbegin() + insertAt, entities.cbegin() + start, entities.cend());
	id = entities.back();
	size_t end = std::min(sortedEntities.size(), insertAt + (size_t)id + 1);
	std::sort(sortedEntities.begin() + insertAt, sortedEntities.begin() + end);

}
void EntityCollection::AddEntity(EntityId id) {
	auto end = sortedEntities.crend();
	for (auto i = sortedEntities.crbegin(); i != end; ++i)
	{
		if (*i < id) {
			sortedEntities.insert(i.base(), id);
			return;
		}
	}

	sortedEntities.insert(sortedEntities.begin(), id);
}


static const std::vector<EntityId>* s;
static int iter = 0;

static bool RemoveIf(EntityId id) {
	auto& scratch = *s;
	if (iter < scratch.size() && scratch[iter] == id)
	{
		++iter;
		return true;
	}
	return false;

}

void EntityCollection::RemoveSortedEntities(const std::vector<EntityId>& deleteEntities, int offset) {

	if (!deleteEntities.size())
		return;

	s = &deleteEntities;
	iter = offset;

	sortedEntities.erase(std::remove_if(sortedEntities.begin(), sortedEntities.end(), RemoveIf), sortedEntities.end());

}
bool EntityCollection::RemoveEntity(EntityId id) {

	auto end = sortedEntities.cend();
	for (auto i = sortedEntities.cbegin(); i != end; ++i)
	{
		if (*i == id) {
			sortedEntities.erase(i);
			return true;
		}
	}

	return false;
}


EntityManagerCollection::EntityManagerCollection()
{
	freeEntities.reserve(usedEntities.size());
	sortedEntities.reserve(usedEntities.size());

	memset(usedEntities.data(), false, sizeof(bool) * usedEntities.size());

	EntityId id = usedEntities.size();
	for (int i = 0; i < usedEntities.size(); i++) {
		freeEntities.push_back(--id);
	}
}

void EntityManagerCollection::NewEntities(unsigned size, std::vector<EntityId>& outIds)
{
	if (freeEntities.size() < size)
		EXCEPTION("No %i free entities left (%i free)!", size, freeEntities.size());

	int start = outIds.size();

	auto iter = freeEntities.cend();

	for (unsigned i = 0; i < size; ++i) {
		auto newEntity = --iter;
		EntityId id = *newEntity;
		outIds.push_back(*newEntity);
		usedEntities[Entity::ToIndex(id)] = true;
	}

	freeEntities.erase(freeEntities.cend() - size, freeEntities.cend());

	sortedEntities.AddSortedEntities(outIds, start);
}

EntityId EntityManagerCollection::NewEntity() {
	if (freeEntities.size() == 0)
		EXCEPTION("No more free entities left!");

	auto newEntity = freeEntities.end() - 1;
	EntityId id = *newEntity;
	freeEntities.erase(newEntity);

	usedEntities[Entity::ToIndex(id)] = true;

	sortedEntities.AddEntity(id);

	return id;
}

void EntityManagerCollection::DeleteSortedEntities(std::vector<EntityId>& deleteEntities)
{
	if (deleteEntities.size() == 0)
		return;

	EntityId id = deleteEntities.front();
	if (id <= Entity::None || id > Entity::MaxEntities ||
		!usedEntities[Entity::ToIndex(id)])
		EXCEPTION("Tried to delete invalid entity id i%!", id);

	id = deleteEntities.back();
	if (id <= Entity::None || id > Entity::MaxEntities ||
		!usedEntities[Entity::ToIndex(id)])
		EXCEPTION("Tried to delete invalid entity id %i!", id);


	for (EntityId id : deleteEntities) {
		usedEntities[Entity::ToIndex(id)] = false;
	}

	//======================= Sorted Entities Delete ===========================

	sortedEntities.RemoveSortedEntities(deleteEntities);

	//======================= Free Entities Insert ============================

	std::reverse(deleteEntities.begin(), deleteEntities.end());

	id = deleteEntities.front();
	unsigned insertAt = freeEntities.size();

	if (freeEntities.size() && id > freeEntities.back())
	{
		auto fend = freeEntities.size();
		for (unsigned i = 0; i < fend; ++i) {
			if (freeEntities[i] < id) {
				insertAt = i;
				break;
			}
		}
	}

	freeEntities.insert(freeEntities.begin() + insertAt, deleteEntities.begin(), deleteEntities.end());
	size_t end = std::min(freeEntities.size(), insertAt + (size_t)id + 1);
	std::sort(freeEntities.begin() + insertAt, freeEntities.begin() + end, std::greater<EntityId>());
}

void EntityManagerCollection::DeleteEntity(EntityId id) {
	if (id <= Entity::None || id > Entity::MaxEntities ||
		!usedEntities[Entity::ToIndex(id)])
		EXCEPTION("Tried to delete invalid entity id %i!", id);

	usedEntities[Entity::ToIndex(id)] = false;

	if (!freeEntities.size() || id < freeEntities.back())
		freeEntities.push_back(id);
	else {
		auto fend = freeEntities.cend();
		for (auto i = freeEntities.cbegin(); i != fend; ++i) {
			if (*i < id) {
				freeEntities.insert(i, id);
				break;
			}
		}
	}

	if (sortedEntities.RemoveEntity(id))
		EXCEPTION("Deleting entity %i was not found!", id);

}

void EntityManagerCollection::ClearEntities()
{
	sortedEntities.clear();

	memset(usedEntities.data(), false, sizeof(bool) * usedEntities.size());

	freeEntities.clear();

	EntityId id = usedEntities.size();
	for (int i = 0; i < usedEntities.size(); i++) {
		freeEntities.push_back(--id);
	}
}
