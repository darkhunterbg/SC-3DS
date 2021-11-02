#pragma once

#include "Entity.h"
#include <vector>
#include <algorithm>
#include <Util.h>

template <class TComponent>
class EntityComponentMap {

	std::array<short, Entity::MaxEntities> _entityToIndexMap;
	std::vector<EntityId> _entities;
	std::vector<TComponent> _components;
	EntityId _maxEntity = Entity::None;

	static constexpr const int InitialMemoryUsage = 4096;

public:
	EntityComponentMap()
	{
		for (int i = 0; i < _entityToIndexMap.size(); ++i)
			_entityToIndexMap[i] = -1;

		int size = sizeof(TComponent);
		if (size == 0) size = 1;
		int count = InitialMemoryUsage / size;
		_components.reserve(count);
		_entities.reserve(count);
	}

	inline short GetIndex(EntityId id) const { return _entityToIndexMap[id]; }
	inline void GetIndex(Span<EntityId> ids, std::vector<short> outIndexes) const
	{
		for (EntityId id : ids) outIndexes.push_back(GetIndex(id));
	}
	inline TComponent& GetComponent(EntityId id) { return  _components[_entityToIndexMap[Entity::ToIndex(id)]]; }
	inline const TComponent& GetComponent(EntityId id) const { return  _components[_entityToIndexMap[Entity::ToIndex(id)]]; }
	inline TComponent* TryGetComponent(EntityId id)
	{
		short index = _entityToIndexMap[Entity::ToIndex(id)];
		return index < 0 ? nullptr : _components[index];
	}
	inline const TComponent* TryGetComponent(EntityId id) const
	{
		short index = _entityToIndexMap[Entity::ToIndex(id)];
		return index < 0 ? nullptr : _components[index];
	}
	inline bool HasComponent(EntityId id) const
	{
		return _entityToIndexMap[Entity::ToIndex(id)] != -1;
	}
	inline auto begin()
	{
		return _components.begin();
	}
	inline auto end()
	{
		return _components.end();
	}
	inline const std::vector<EntityId>& GetEntities() const { return _entities; }
	inline std::vector<TComponent>& GetComponents() { return _components; }
	inline const std::vector<TComponent>& GetComponents() const { return _components; }

	void NewComponent(EntityId id)
	{
		short index = _components.size();

		_components.push_back(TComponent());
		_entities.push_back(id);
		_entityToIndexMap[Entity::ToIndex(id)] = index;

		if (id > _maxEntity) _maxEntity = id;
	}
	void NewComponents(const std::vector<EntityId>& ids)
	{
		if (ids.size() == 0) return;

		short index = _components.size();

		_components.resize(index + ids.size());
		_entities.emplace(_entities.end(), ids.begin(), ids.end());

		for (EntityId id : ids)
			_entityToIndexMap[Entity::ToIndex(id)] = index++;

		EntityId max = *std::max_element(ids.cbegin(), ids.cend());

		if (max > _maxEntity) _maxEntity = max;
	}
	void DeleteComponent(EntityId id)
	{
		short index = _entityToIndexMap[Entity::ToIndex(id)];
		_entityToIndexMap[Entity::ToIndex(id)] = -1;
		_components.erase(_components.begin() + index);
		_entities.erase(_entities.begin() + index);

		int end = Entity::ToIndex(_maxEntity);
		EntityId last = Entity::None;

		for (int i = 0; i < end; ++i)
		{
			if (_entityToIndexMap[i] > index)
			{
				--_entityToIndexMap[i];
				last = Entity::ToId(i);
			}
		}

		_maxEntity = last;
	}


	void DeleteComponents(const std::vector<EntityId>& ids)
	{
		if (ids.size() == 0) return;


		Util::_delIndexesScratch.clear();

		for (EntityId id : ids)
		{
			short index = _entityToIndexMap[Entity::ToIndex(id)];
			if (index == -1)
				continue;
			Util::_delIndexesScratch.push_back(index);
			_entityToIndexMap[Entity::ToIndex(id)] = -1;
		}

		if (Util::_delIndexesScratch.size() == 0) return;



		std::sort(Util::_delIndexesScratch.begin(), Util::_delIndexesScratch.end(), std::greater<short>());

		for (short index : Util::_delIndexesScratch)
		{
			_components.erase(_components.begin() + index);
			_entities.erase(_entities.begin() + index);
		}


		EntityId minEntity = Entity::None;
		if (_entities.size() > 0)
		{
			_maxEntity = *std::max_element(_entities.begin(), _entities.end());
			minEntity = *std::min_element(_entities.begin(), _entities.end());
		}
		else
			_maxEntity = Entity::None;

		//std::sort(Util::_delIndexesScratch.begin(), Util::_delIndexesScratch.end(), std::less<short>());
		
		if (_entities.size() == 0) return;

		int start = Entity::ToIndex(minEntity);
		int end = Entity::ToIndex(_maxEntity);

		int iter = 0;
		for (short index : Util::_delIndexesScratch)
		{
			index -= iter++;
			for (int i = start; i <= end; ++i)
			{
				if (_entityToIndexMap[i] > index)
					--_entityToIndexMap[i];
			}
		}

		// TODO: full validation check 
	}

	size_t GetMemoryUsage() const
	{
		size_t size = sizeof(_entityToIndexMap);
		size += _entities.capacity() * sizeof(EntityId);
		size += _components.capacity() * sizeof(TComponent);

		return size;
	}
};

