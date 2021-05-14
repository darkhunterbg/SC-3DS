#pragma once

#include <vector>
#include <array>
#include <cstring>
#include <bitset>

#include "Entity.h"
#include "../Span.h"
#include "../MathLib.h"
#include "../Color.h"
#include "../Assets.h"
#include "../Data/UnitDef.h"

#include "Debug.h"



////template <unsigned id>
//struct IComponent {
//	//static constexpr const unsigned ComponentId = id;
//};


template <class TComponent, unsigned MaxComponents = Entity::MaxEntities>
class ComponentCollection
{
private:
	//std::array<bool, MaxComponents> hasComponent;
	std::array<TComponent, MaxComponents> components;
public:

	ComponentCollection() {
		//memset(hasComponent.data(), false, MaxComponents * sizeof(bool));
	}

	ComponentCollection(const ComponentCollection&) = delete;
	ComponentCollection& operator=(const ComponentCollection&) = delete;

	TComponent& NewComponent(EntityId id) {
		int index = Entity::ToIndex(id);
		/*if (hasComponent[index])
			EXCEPTION("Entity %i already has component!", id);

		hasComponent[index] = true;*/
		components[index] = TComponent();
		return components[index];
	}

	TComponent& NewComponent(EntityId id, const TComponent& component) {
		int index = Entity::ToIndex(id);
		//if (hasComponent[index])
		//	EXCEPTION("Entity %i already has component!", id);

		//hasComponent[index] = true;
		components[index] = component;
		return components[index];
	}

	/*void RemoveComponent(EntityId id) {
		int index = Entity::ToIndex(id);
		if (!hasComponent[index])
			EXCEPTION("Entity %i does not have component!", id);

		hasComponent[index] = false;

	}
	inline bool HasComponent(EntityId id) const {
		return hasComponent[Entity::ToIndex(id)];
	}*/

	inline TComponent& GetComponent(EntityId id) {
		return components[Entity::ToIndex(id)];
	}
	inline TComponent& GetComponent(EntityId id) const {
		return components[Entity::ToIndex(id)];
	}
	/*inline const TComponent* TryGetComponent(EntityId id) const {
		if (!hasComponent[Entity::ToIndex(id)])
			return nullptr;
		return components[Entity::ToIndex(id)];
	}*/

	inline TComponent& at(int i) { return components[i]; };
	inline const TComponent& at(int i) const { return components[i]; }
	inline TComponent& operator[](int i) { return components[i]; }
	inline const TComponent& operator[](int i) const { return components[i]; }
	inline size_t size() const { return 0; }
};

enum class ComponentFlags {
	PositionChanged = 0,
	RenderChanged = 1,
	UnitRenderChanged = 2,
	UnitOrientationChanged = 3,
	AnimationEnabled = 4,
	AnimationFrameChanged = 5,
	UnitAnimationFrameChanged = 6,
	NavigationWork = 7,
	UpdateTimers = 8,
};

struct FlagsComponent {
	std::bitset<32> value;
	inline bool test(ComponentFlags flag) const {
		return value.test((int)flag);
	}
	inline void set(ComponentFlags flag, bool v = true) {
		value.set((int)flag, v);
	}
	inline void clear(ComponentFlags flag) {
		value.set((int)flag, false);
	}
};

struct RenderComponent {
	Image sprite;
	int8_t depth = 0;
	bool hFlip = false;
};

struct RenderUnitComponent {
	Color4 unitColor;
	Image sprite;
	Image shadowSprite;
	Image colorSprite;
	int depth = 0;
	bool hFlip = false;
};


struct RenderUnitOffsetComponent {
	Vector2Int16 offset;
	Vector2Int16 shadowOffset;
};

struct RenderUnitDestinationComponent {
	Vector2Int16 dst;
	Vector2Int16 shadowDst;
};


struct NavigationComponent {
	Vector2Int16 target;
	uint8_t targetHeading;
};

struct MovementComponent {
	Vector2Int8 velocity;
};

struct UnitComponent {
	const UnitDef* def;
};

struct UnitMovementComponent {
	uint8_t movementSpeed;
	uint8_t rotationSpeed;

	inline void FromDef(const UnitDef& def) {
		movementSpeed = def.MovementSpeed;
		rotationSpeed = def.RotationSpeed;
	}
};


struct AnimationTrackerComponent {
	int8_t clipFrame = 0;
	uint8_t frameCountdown = 1;
	uint8_t totalFrames = 0;
	uint8_t frameTime = 1;
	bool looping = false;

	inline void Restart() {
		clipFrame = -1;
		frameCountdown = frameTime;
	}

	inline void PlayClip(const AnimationClip* clip) {
		if (clip) {
			totalFrames = clip->GetFrameCount();
			looping = clip->looping;
			clipFrame = -1;
			frameCountdown = frameTime = clip->frameTime;
		}
		else {
			totalFrames = 0;
			clipFrame = 0;
		}
	}
};

struct UnitAnimationTrackerComponent {
	int8_t clipFrame = 0;
	uint8_t frameCountdown = 1;
	uint8_t totalFrames = 0;
	uint8_t frameTime = 1;
	bool looping = false;

	inline void Restart() {
		clipFrame = -1;
		frameCountdown = frameTime;
	}

	inline void PlayClip(const UnitAnimationClip* clip) {
		if (clip) {
			totalFrames = clip->GetFrameCount();
			looping = clip->looping;
			clipFrame = -1;
			frameCountdown = frameTime = clip->frameTime;
		}
		else {
			totalFrames = 0;
			clipFrame = 0;
		}
	}
};

struct UnitAnimationComponent {
	const UnitAnimationClip* clip;
};

struct AnimationComponent {
	const AnimationClip* clip;
};


struct ColliderComponent {
	Rectangle16 collider;
};


enum class TimerExpiredAction {
	DeleteEntity = 0,
};

struct TimingComponent {
	std::array<uint16_t, 4 > timers;
	std::array<TimerExpiredAction, 4 > actions;
	uint8_t activeTimers = 0;

	inline void NewTimer(uint16_t durationFrames, TimerExpiredAction action) {

		timers[activeTimers] = durationFrames;
		actions[activeTimers] = action;

		++activeTimers;
	}
};