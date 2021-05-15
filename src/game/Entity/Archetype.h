#pragma once

#include "Entity.h"
#include "../Span.h"
#include <vector>


class EntityArchetype {
private:
	std::vector<EntityId> entities;
	std::vector<EntityId> newEntities;
	std::vector<EntityId> removedEntities;
	std::array<bool, Entity::MaxEntities> hasEntity;
	std::vector<EntityId> scratch;
	const char* name;
public:
	EntityArchetype(const EntityArchetype&) = delete;
	EntityArchetype& operator=(const EntityArchetype&) = delete;

	EntityArchetype(const char* name);

	inline const Span<EntityId> GetEntities() const {
		return { entities.data(), entities.size() };
	}
	inline bool HasEntity(EntityId id) const {
		return hasEntity[Entity::ToIndex(id)];
	}
	inline const Span< EntityId> NewEntities() const {
		return { newEntities.data(), newEntities.size() };
	}
	inline const Span< EntityId> RemovedEntities() const {
		return { removedEntities.data(), removedEntities.size() };
	}
	void AddEntity(EntityId id);
	void AddEntities(std::vector<EntityId>& entities, bool sorted);
	void RemoveEntity(EntityId id);
	int RemoveEntities(std::vector<EntityId>& del, bool sorted);
	void ClearEntities();
	void CommitChanges();
};
