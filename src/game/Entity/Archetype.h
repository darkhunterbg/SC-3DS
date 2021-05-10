#pragma once

#include "Entity.h"
#include "Component.h"
#include <vector>


class EntityArchetype {
private:
	std::vector<EntityId> entities;
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
	void AddEntity(EntityId id);
	void RemoveEntity(EntityId id);
};
