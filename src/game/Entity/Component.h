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

#include "Common.h"

template <class TComponent, unsigned MaxComponents = Entity::MaxEntities>
class ComponentCollection
{
private:
	std::array<TComponent, MaxComponents> components;
public:

	ComponentCollection() {
		//memset(hasComponent.data(), false, MaxComponents * sizeof(bool));
	}

	ComponentCollection(const ComponentCollection&) = delete;
	ComponentCollection& operator=(const ComponentCollection&) = delete;

	TComponent& NewComponent(EntityId id) {
		int index = Entity::ToIndex(id);
		components[index] = TComponent();
		return components[index];
	}

	TComponent& NewComponent(EntityId id, const TComponent& component) {
		int index = Entity::ToIndex(id);
		components[index] = component;
		return components[index];
	}


	inline void CopyComponent(EntityId from, EntityId to) {
		components[Entity::ToIndex(to)] = components[Entity::ToIndex(from)];
	}
	inline TComponent& GetComponent(EntityId id) {
		return components[Entity::ToIndex(id)];
	}
	inline const TComponent& GetComponent(EntityId id) const {
		return components[Entity::ToIndex(id)];
	}

	inline TComponent& at(int i) { return components[i]; };
	inline const TComponent& at(int i) const { return components[i]; }
	inline TComponent& operator[](int i) { return components[i]; }
	inline const TComponent& operator[](int i) const { return components[i]; }
	inline size_t size() const { return 0; }

};


enum class ComponentFlags {
	PositionChanged = 0,
	RenderEnabled = 1,
	RenderChanged = 2,
	OrientationChanged = 3,
	//AnimationWait = 4,
	AnimationEnabled = 5,
	//AnimationFrameChanged = 6,
	NavigationWork = 7,
	UpdateTimers = 8,
	UnitStateChanged = 9,
	SoundTrigger = 10,
	SoundMuted = 11,
	UnitAIPaused = 12,
	UnitAIStateChanged = 13,
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
	inline void RemoveChild(EntityId child) {
		int index = 0xFF;
		for (int i = 0; i < childCount; ++i) {
			if (children[i] == child) {
				index = i;
				break;
			}
		}

		for (int i =index; i < childCount - 1; ++i) {
			children[i] = children[i + 1];
		}
		--childCount;
	}
};

struct RenderComponent {
	ImageFrame sprite;
	const ImageFrame* colorMask = nullptr;
	Color32 color = 0xFFFFFFFF;
	int8_t depth = 0;
	bool hFlip = false;
};

struct RenderDestinationComponent {
	Vector2Int16 dst;
	int order = 0;
};

struct AnimationComponent {
	const Image* baseImage;
	uint8_t instructionStart;
	uint8_t instructionEnd;
};

struct AnimationStateComponent {
	uint32_t instructionId;
	uint16_t animFrame = 0;
	uint8_t wait;
};

struct NavigationComponent {
	Vector2Int16 target;
	uint8_t targetHeading;
};

struct MovementComponent {
	Vector2Int8 velocity;
};

struct UnitStateDataComponent {
	union {
		EntityId entityId;
		Vector2Int16 position;
	} target = { 0 };

	union {
		EntityId miningEffect;
	} other;
};


struct UnitAIStateDataComponent {
	union {
		EntityId entityId;
		Vector2Int16 position;
	} target = { 0 };

	union {
		EntityId entityId;
		Vector2Int16 position;
	} target2 = { 0 };
};


struct UnitComponent {
	const UnitDef* def;
	EntityId movementGlowEntity = Entity::None;
	EntityId cargo = Entity::None;;
	EntityId fires[3] = { Entity::None, Entity::None, Entity::None };
	uint16_t kills = 0;

	inline bool HasMovementGlow() const { return movementGlowEntity != Entity::None; }
	inline bool HasCargo() const { return cargo != Entity::None; }
};

struct UnitDataComponent {
	uint8_t supplyUsage = 0;
	uint8_t supplyProvides = 0;
	uint8_t vision = 1;
	bool isBuilding = false;
	uint8_t internalTimer = 0;

	std::array<const UnitDef*, 5> productionQueue;
	uint8_t queueSize = 0;
	uint16_t queueTimer = 0;
	Vector2Int16 spawnOffset;
	bool build = false;

	uint16_t resources = 0;

	inline void ResetFireTimer() {
		internalTimer = 18;
	}
	inline bool EnqueueProduce(const UnitDef& def) {
		if (IsQueueFull()) {
			return false;
		}

		//productionQueue[queueSize] = &def;

		//++queueSize;
		//if (queueTimer == 0) {
		//	queueTimer = def.BuildTime;
		//	build = false;
		//}

		return true;
	}

	inline bool IsQueueEmpty() const {
		return queueSize == 0;
	}
	inline bool IsQueueFull() const {
		return queueSize == productionQueue.size();
	}
	inline bool TickQueue(int counter = 1) {
		queueTimer = std::max(0, (int)queueTimer - counter);
		return queueTimer == 0;
	}

	inline const UnitDef* Dequeue() {
		const UnitDef* result = productionQueue[0];
	/*	for (int i = 1; i < queueSize; ++i) {
			productionQueue[i - 1] = productionQueue[i];
		}

		--queueSize;

		if (queueSize) {
			queueTimer = productionQueue[0]->BuildTime;
		}
		
		build = false;*/

		return result;
	}
	inline const UnitDef* RemoveFromQueue(int queuePos) {
		if (queuePos >= queueSize)
			return nullptr;

		return nullptr;

		/*const UnitDef* result = productionQueue[queuePos];

		for (int i = queuePos + 1; i < queueSize; ++i) {
			productionQueue[i - 1] = productionQueue[i];
		}

		--queueSize;

		if (queueSize) {
			if (queuePos == 0) {
				queueTimer = productionQueue[0]->BuildTime;
				build = false;
			}
		}
		else {
			queueTimer = 0;
		}

		return result;*/
	}

	inline void FromDef(const UnitDef& def) {
	/*	supplyUsage = def.UseSupplyDoubled;
		supplyProvides = def.ProvideSupplyDoubled;
		vision = def.Vision + 1;
		isBuilding = def.IsBuilding;
		if (def.IsResourceContainer)
			resources = 1500;
		spawnOffset = def.SpawnOffset;
		queueSize = 0;
		queueTimer = 0;*/
		ResetFireTimer();
	}

};

struct UnitHealthComponent {
	uint16_t current = 1;
	uint16_t max = 1;
	uint8_t armor = 0;

	inline bool IsInvulnerable() const { return max == 0xFFFF; }

	inline bool Reduce(uint16_t value) {
		if (IsInvulnerable())
			return 0;

		uint16_t dmg = std::max((int)value - (int)armor, 1);
		uint16_t damage = std::min(dmg, current);
		bool kill = damage == current;
		current -= damage;
		return damage && kill;
	}
	inline bool ReduceUnmitigated(uint16_t value) {
		if (IsInvulnerable())
			return 0;

		uint16_t damage = std::min(value, current);
		bool kill = damage == current;
		current -= damage;
		return damage && kill;
	}

	inline bool AtMax() const { return current == max; }

	inline bool IsDead() const { return current == 0; }
	inline void FromDef(const UnitDef& def) {
		current = max = def.Health;
		//armor = def.Armor;
	}
};

struct UnitMovementComponent {
	uint8_t movementSpeed;
	uint8_t rotationSpeed;

	inline void FromDef(const UnitDef& def) {
		/*movementSpeed = def.MovementSpeed;
		rotationSpeed = def.RotationSpeed;*/
	}
};

struct UnitWeaponComponent {
	uint16_t damage = 1;

	uint8_t maxRange = 1;
	uint8_t cooldown = 1;

	uint8_t remainingCooldown = 0;

	inline void FromDef(const WeaponDef& def) {
		maxRange = def.MaxRange;
		cooldown = def.Cooldown;
		damage = def.Damage;
	}

	inline void StartCooldown() {
		remainingCooldown = cooldown;
	}
	inline bool IsReady() const { return remainingCooldown == 0; }
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


struct TimingActionComponent {
	TimerExpiredAction action = TimerExpiredAction::None;
};


struct SoundSourceComponent {
	const AudioClip* clip;
	uint16_t priority = 0;
};