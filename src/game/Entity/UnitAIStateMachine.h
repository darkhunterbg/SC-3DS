#pragma once

#include <vector>
#include "Entity.h"

class EntityManager;

struct UnitAIEnterStateData {
	std::vector<EntityId> entities;

	inline void clear()
	{
		entities.clear();
	}
	inline size_t size() const { return entities.size(); }
};

struct UnitAIThinkData {
	std::vector<EntityId> entities;

	inline void clear()
	{
		entities.clear();
	}
	inline size_t size() const { return entities.size(); }
};

class IUnitAIState {
public:
	virtual void Think(UnitAIThinkData& data, EntityManager& em) = 0;
	virtual void EnterState(UnitAIEnterStateData& data, EntityManager& em);
public:
	static std::vector<IUnitAIState*> States;
};

namespace UnitAIStates
{
	class IdleAggressive :public IUnitAIState {
		virtual void EnterState(UnitAIEnterStateData& data, EntityManager& em) override;
		virtual void Think(UnitAIThinkData& data, EntityManager& em) override;
	};

	class IdlePassive :public IUnitAIState {
		virtual void EnterState(UnitAIEnterStateData& data, EntityManager& em) override;
		virtual void Think(UnitAIThinkData& data, EntityManager& em) override;
	};

	class AttackTarget : public IUnitAIState {
		virtual void Think(UnitAIThinkData& data, EntityManager& em) override;
	};

	class AttackLoop : public IUnitAIState {
		virtual void EnterState(UnitAIEnterStateData& data, EntityManager& em) override;
		virtual void Think(UnitAIThinkData& data, EntityManager& em) override;
	};
}