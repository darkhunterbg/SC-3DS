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
	void RemoveEntity(EntityId id);

	void CommitChanges();
};
