#pragma once

#include "../MathLib.h"
#include "../Assets.h"
#include "Component.h"
#include "Entity.h"

class EntityManager;

class TimeUtil {
private:
	TimeUtil() = delete;
	~TimeUtil() = delete;
public:

	inline static constexpr const TimingDuration FrameTime(int frames) {
		return frames;
	}

	inline static constexpr const TimingDuration SecondsTime(float seconds) {
		return (uint16_t)(seconds * 24);
	}
};

class EntityUtil {
private:
	EntityUtil() = delete;
	~EntityUtil() = delete;

	static EntityManager* emInstance;

	friend class EntityManager;

	static inline  EntityManager& GetManager() { return *emInstance; }
public:
	/// <summary>
	/// Start new expiration timer on an entity (doesnt't need to be from Timing archetype before)
	/// </summary>
	static void StartTimer(EntityId e, uint16_t time, TimerExpiredAction action, bool looping = false);

	static void StartTimerMT(EntityId e, uint16_t time, TimerExpiredAction action, bool looping = false);

	/// <summary>
	/// Set current position (will cause render update)
	/// </summary>
	static void SetPosition(EntityId e, Vector2Int16 pos);

	static void SetImageFrame(EntityId e, const Image& img, unsigned frameId, bool hFlip);

	static void SetMapObjectBoundingBoxFromRender(EntityId e);

	static void CopyFlag(EntityId from, EntityId to, ComponentFlags flag);

	static uint8_t GetOrientationToPosition(EntityId id, Vector2Int16 pos);

	static void SetOrientation(EntityId id, int orientation);

	static void UpdateAnimationVisual(EntityId id);
};

class UnitEntityUtil {
private:
	UnitEntityUtil() = delete;
	~UnitEntityUtil() = delete;

	static EntityManager* emInstance;

	friend class EntityManager;

	static inline  EntityManager& GetManager() { return *emInstance; }
public:
	static EntityId NewUnit(const UnitDef& def, PlayerId player, Vector2Int16 position, EntityId id = Entity::None);

	static void AttackTarget(EntityId id, EntityId target);

	static void GoTo(EntityId id, Vector2Int16 pos);

	static void SetAIState(EntityId id, UnitAIState state);
	static void SetAIState(EntityId id, UnitAIState state, Vector2Int16 target);
	static void SetAIState(EntityId id, UnitAIState state, EntityId target);

	static bool IsAlly(PlayerId player, EntityId id);
	static bool IsEnemy(PlayerId player, EntityId id);
};