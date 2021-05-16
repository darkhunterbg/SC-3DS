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

	/// <summary>
	/// Set current position (will cause render update)
	/// </summary>
	static void SetPosition(EntityId e, Vector2Int16 pos);

	/// <summary>
	/// Copies Unit.Render archetype to Render archetype
	/// </summary>
	static void CopyUnitRenderSettings(EntityId e);

	/// <summary>
	/// Start regular animation
	/// </summary>
	static void PlayAnimation(EntityId e, const AnimationClip& clip);
	
	/// <summary>
	/// Start unit animation
	/// </summary>
	static void PlayAnimation(EntityId e, const UnitAnimationClip& clip);

	/// <summary>
	/// Set render to show a frame from animation clip
	/// </summary>
	static void SetRenderFromAnimationClip(EntityId e, const AnimationClip& clip, uint8_t frame);

	/// <summary>
	/// Set render to show a frame from unit animation clip
	/// </summary>
	static void SetRenderFromAnimationClip(EntityId e, const UnitAnimationClip& clip, uint8_t frame);
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
};