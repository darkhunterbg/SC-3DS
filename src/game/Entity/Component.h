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

	inline void CopyComponent(EntityId from, EntityId to) {
		components[Entity::ToIndex(to)] = components[Entity::ToIndex(from)];
	}
	inline TComponent& GetComponent(EntityId id) {
		return components[Entity::ToIndex(id)];
	}
	inline const TComponent& GetComponent(EntityId id) const {
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

typedef uint8_t PlayerId;

enum class ComponentFlags {
	PositionChanged = 0,
	RenderEnabled = 1,
	RenderChanged = 2,
	OrientationChanged = 3,
	AnimationSetChanged = 4,
	AnimationEnabled = 5,
	AnimationFrameChanged = 6,
	NavigationWork = 7,
	UpdateTimers = 8,
	UnitStateChanged = 9,
	SoundTrigger = 10,
	SoundMuted = 11,
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

struct ChildComponent {
	std::array<EntityId, 4> children;
	uint8_t childCount = 0;

	inline void AddChild(EntityId child) {
		children[childCount++] = child;
	}
};

struct RenderComponent {
	Image sprite;
	int8_t depth = 0;
	bool hFlip = false;

	inline void SetSpriteFrame(const SpriteFrame& s) {
		sprite = s.sprite.image;
		hFlip = s.hFlip;
	}
};

struct RenderUnitComponent {
	Color32 unitColor;
	Image sprite;
	Image shadowSprite;
	Image colorSprite;
	int depth = 0;
	bool hFlip = false;

	inline void SetSpriteFrame(const UnitSpriteFrame& s) {
		sprite = s.sprite.image;
		shadowSprite = s.shadowSprite.image;
		colorSprite = s.colorSprite.image;
		hFlip = s.hFlip;
	}
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

enum class UnitState : uint8_t {
	Idle = 0,
	Turning = 1,
	Movement = 2,
	Attacking = 3,
	Death = 4,
};

static constexpr const int UnitStatesCount = 5;


struct UnitComponent {
	const UnitDef* def;
	EntityId movementGlowEntity = Entity::None;

	inline bool HasMovementGlow() const { return movementGlowEntity != Entity::None; }
};

struct UnitDataComponent {
	uint8_t supplyUsage = 0;
	uint8_t supplyProvides = 0;
	uint8_t visiion = 1;
	bool isBuilding = false;

	inline void FromDef(const UnitDef& def) {
		supplyUsage = def.UseSupplyDoubled;
		supplyProvides = def.ProvideSupplyDoubled;
		visiion = def.Vision + 1;
		isBuilding = def.IsBuilding;
	}
};

struct UnitMovementComponent {
	uint8_t movementSpeed;
	uint8_t rotationSpeed;

	inline void FromDef(const UnitDef& def) {
		movementSpeed = def.MovementSpeed;
		rotationSpeed = def.RotationSpeed;
	}
};

struct AnimationOrientationComponent {
	std::array<const AnimationClip*, 32> clips;
	inline const AnimationClip& GetClip(int pos) const { return *clips[pos]; }

	inline void CopyArray(const AnimationClip(&a)[32]) {
		for (int i = 0; i < 32; ++i) {
			clips[i] = &a[i];
		}
	}

	static_assert(sizeof(clips) == 32 * sizeof(const AnimationClip*),
		"AnimationOrientationComponent Assert fail");
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
			clipFrame = 0;
			frameCountdown = frameTime = clip->frameTime;
		}
		else {
			totalFrames = 0;
			clipFrame = 0;
			frameCountdown = 1;
		}
	}

	inline void PlayClip(const UnitAnimationClip* clip) {
		if (clip) {
			totalFrames = clip->GetFrameCount();
			looping = clip->looping;
			clipFrame = 0;
			frameCountdown = frameTime = clip->frameTime;
		}
		else {
			totalFrames = 0;
			clipFrame = 0;
			frameCountdown = 1;
		}
	}
};

struct UnitAnimationOrientationComponent {
	std::array<const UnitAnimationClip*, 32> clips;
	inline const UnitAnimationClip& GetClip(int pos) const { return *clips[pos]; }
	inline void CopyArray(const UnitAnimationClip(&a)[32]) {
		for (int i = 0; i < 32; ++i) {
			clips[i] = &a[i];
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

typedef uint16_t TimingDuration;

struct TimingComponent {
	TimingDuration timer = 0;
	TimingDuration nextTimer = 0;

	inline void NewTimer(TimingDuration duration, bool looping = false) {

		timer = duration;
		nextTimer = looping * duration;
	}

};

enum class TimerExpiredAction : uint8_t {
	None = 0,
	WeaponAttack = 1,
	UnitDeathAfterEffect = 2,
	DeleteEntity = 3,

};


struct TimingActionComponent {
	TimerExpiredAction action = TimerExpiredAction::None;

	static constexpr const int ActionTypeCount = 4;
};


struct SoundSourceComponent {
	AudioClip clip;
	uint16_t priority = 0;
};