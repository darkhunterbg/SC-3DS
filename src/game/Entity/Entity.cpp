#include "Entity.h"
#include "../Platform.h"
#include <algorithm>

EntityCollection::EntityCollection()
{
	for (int i = 0; i < entityToIndexMap.size(); ++i)
		entityToIndexMap[i] = -1;
}

EntityId EntityCollection::NewEntity() {
	if (entities.size() == Entity::MaxEntities)
		EXCEPTION("No more free entities left!");

	++lastId;
	int index = entities.size();
	entityToIndexMap[Entity::ToIndex(lastId)] = index;
	entities.push_back(lastId);


	auto end = sortedEntities.crend();
	for (auto i = sortedEntities.crbegin(); i != end; ++i)
	{
		if (*i < lastId) {
			sortedEntities.insert(i.base(), lastId);
			return lastId;
		}
	}

	sortedEntities.insert(sortedEntities.begin(), lastId);

	return lastId;
}
void EntityCollection::DeleteEntity(EntityId id) {
	if (id == Entity::None || id > lastId)
		EXCEPTION("Tried to delete invalid entity!");

	int index = entityToIndexMap[Entity::ToIndex(id)];
	entityToIndexMap[Entity::ToIndex(id)] = -1;
	entities.erase(entities.begin() + index);

	int size = entities.size();
	for (int i = index; i < size; ++i) {
		EntityId eid = entities[i];
		entityToIndexMap[Entity::ToIndex(eid)] -= 1;
	}

	auto end = sortedEntities.cend();
	for (auto i = sortedEntities.cbegin(); i != end; ++i)
	{
		if (*i == id) {
			sortedEntities.erase(i);
			return;
		}
	}

	EXCEPTION("Deleting entity %id was not found!", id);
		
}
