#pragma once

#include <vector>
#include <array>
#include <cstddef>
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
	static inline constexpr const EntityId ToId(int  index) { return (short)(index); };
};


class EntityCollection {
	std::vector<EntityId> sortedEntities;
public:
	EntityCollection(const EntityCollection&) = delete;
	EntityCollection& operator=(const EntityCollection&) = delete;

	EntityCollection(unsigned reserve = 0);

	void reserve(unsigned reserve);
	void clear();

	void AddSortedEntities(const std::vector<EntityId>& outIds, int offset = 0);
	void AddEntity(EntityId id);
	bool RemoveEntity(EntityId id);
	void RemoveSortedEntities(const std::vector<EntityId>& e, int offset = 0);

	inline const std::vector<EntityId>& GetEntities() const { return sortedEntities; }
	inline EntityId GetEntity(int index) const { return sortedEntities[index]; }

	inline EntityId at(int i)const { return GetEntity(i); };
	inline size_t size() const { return sortedEntities.size(); }
	inline EntityId operator[](int i)  const { return GetEntity(i); }
	inline std::vector<EntityId>::const_iterator begin() const
	{
		return sortedEntities.cbegin();
	}
	inline  std::vector<EntityId>::const_iterator end() const
	{
		return sortedEntities.cend();
	}
};



class EntityManagerCollection
{
private:
	EntityCollection sortedEntities;
	std::array<bool, Entity::MaxEntities> usedEntities;
	std::vector<EntityId> freeEntities;
public:
	EntityManagerCollection(const EntityManagerCollection&) = delete;
	EntityManagerCollection& operator=(const EntityManagerCollection&) = delete;

	EntityManagerCollection();

	inline const Span<EntityId> GetEntities() const { return { sortedEntities.GetEntities().data(), sortedEntities.GetEntities().size() }; }
	inline EntityId GetEntity(int index) const { return sortedEntities[index]; }

	void NewEntities(unsigned size, std::vector<EntityId>& outIds);
	EntityId NewEntity();
	void DeleteEntity(EntityId id);
	void DeleteSortedEntities(std::vector<EntityId>& e);
	void ClearEntities();

	inline bool EntityExists(EntityId id) const
	{
		return usedEntities[Entity::ToIndex(id)];
	}

	inline EntityId at(int i)const { return GetEntity(i); };
	inline size_t size() const { return sortedEntities.size(); }
	inline EntityId operator[](int i)  const { return GetEntity(i); }
	inline Span<EntityId>::ConstIterator begin() const
	{
		return GetEntities().begin();
	}
	inline Span<EntityId>::ConstIterator end() const
	{
		return  GetEntities().end();
	}
};
