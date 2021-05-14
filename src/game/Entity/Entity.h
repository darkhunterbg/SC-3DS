#pragma once

#include <vector>
#include <array>
#include "../Span.h"
typedef short EntityId;

class Entity {
private:
	Entity() = delete;
	~Entity() = delete;

public:
	static constexpr const int MaxEntities = 10'000;
	static constexpr const EntityId None = -1;
	static inline constexpr const int ToIndex(EntityId id) { return id; }
	static inline constexpr const EntityId ToId(int  index) { return index; }
};

class EntityCollection
{
private:
	std::vector<EntityId> sortedEntities;
	std::array<bool, Entity::MaxEntities> usedEntities;
	std::vector<EntityId> freeEntities;
public:
	EntityCollection(const EntityCollection&) = delete;
	EntityCollection& operator=(const EntityCollection&) = delete;

	EntityCollection();

	inline const Span<EntityId> GetEntities() const { return { sortedEntities.data(),sortedEntities.size() }; }
	inline EntityId GetEntity(int index) const { return sortedEntities[index]; }

	void NewEntities(unsigned size, std::vector<EntityId>& outIds);
	EntityId NewEntity();
	void DeleteEntity(EntityId id);
	void DeleteEntities(std::vector<EntityId>& e, bool sorted);
	void ClearEntities();

	inline bool EntityExists(EntityId id) const {
		return usedEntities[Entity::ToIndex(id)];
	}

	inline EntityId at(int i)const { return GetEntity(i); };
	inline size_t size() const { return sortedEntities.size(); }
	inline EntityId operator[](int i)  const { return GetEntity(i); }
	inline Span<EntityId>::ConstIterator begin() const {
		return GetEntities().begin();
	}
	inline Span<EntityId>::ConstIterator end() const {
		return  GetEntities().end();
	}
};


