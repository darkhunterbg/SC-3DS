#pragma once

#include "Entity.h"
#include "../Span.h"
#include <vector>


class EntityArchetype {
private:
	EntityCollection entities;
	std::vector<EntityId> newEntities;
	std::vector<EntityId> removedEntities;
	std::array<bool, Entity::MaxEntities> hasEntity;
	std::vector<EntityId> scratch;
	const char* name;
public:
	EntityArchetype(const EntityArchetype&) = delete;
	EntityArchetype& operator=(const EntityArchetype&) = delete;

	EntityArchetype(const char* name);

	inline const std::vector<EntityId>& GetEntities() const {
		return entities.GetEntities(); 
	}
	inline bool HasEntity(EntityId id) const {
		return hasEntity[Entity::ToIndex(id)];
	}
	inline const std::vector<EntityId>& NewEntities() const {
		return  newEntities;
	}
	inline const std::vector<EntityId>& RemovedEntities() const {
		return removedEntities;
	}
	void AddEntity(EntityId id);
	void AddSortedEntities(const std::vector<EntityId>& entities);
	void AddEntities(std::vector<EntityId>& entities, bool sorted = false);
	void RemoveEntity(EntityId id);
	int RemoveEntities(std::vector<EntityId>& del, bool sorted = false);
	int RemoveSortedEntities(const std::vector<EntityId>& del);
	void ClearEntities();
	void CommitChanges();
};
