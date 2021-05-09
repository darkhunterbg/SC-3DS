#pragma once

#include <vector>
#include <array>
#include <cstring>

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
	std::array<bool, MaxComponents> hasComponent;
	std::array<TComponent, MaxComponents> components;
public:

	ComponentCollection() {
		memset(hasComponent.data(), false, MaxComponents * sizeof(bool));
	}

	ComponentCollection(const ComponentCollection&) = delete;
	ComponentCollection& operator=(const ComponentCollection&) = delete;

	TComponent& NewComponent(EntityId id) {
		int index = Entity::ToIndex(id);
		if (hasComponent[index])
			EXCEPTION("Entity %i already has component!", id);

		hasComponent[index] = true;
		components[index] = TComponent();
		return components[index];
	}

	TComponent& NewComponent(EntityId id, const TComponent& component) {
		int index = Entity::ToIndex(id);
		if (hasComponent[index])
			EXCEPTION("Entity %i already has component!", id);

		hasComponent[index] = true;
		components[index] = component;
		return components[index];
	}

	void RemoveComponent(EntityId id) {
		int index = Entity::ToIndex(id);
		if (!hasComponent[index])
			EXCEPTION("Entity %i does not have component!", id);

		hasComponent[index] = false;

	}
	inline bool HasComponent(EntityId id) const {
		return hasComponent[Entity::ToIndex(id)];
	}

	inline TComponent& GetComponent(EntityId id) {
		return components[Entity::ToIndex(id)];
	}
	inline TComponent& GetComponent(EntityId id) const {
		return components[Entity::ToIndex(id)];
	}
	inline const TComponent* TryGetComponent(EntityId id) const {
		if (!hasComponent[Entity::ToIndex(id)])
			return nullptr;
		return components[Entity::ToIndex(id)];
	}

	inline TComponent& at(int i) { return components[i]; };
	inline const TComponent& at(int i) const { return components[i]; }
	inline TComponent& operator[](int i) { return components[i]; }
	inline const TComponent& operator[](int i) const { return components[i]; }
	inline size_t size() const { return 0; }

	// we need component iteration somehow
};

struct EntityChangeComponent {
	bool changed;
};

struct RenderComponent {
	Color4 unitColor;
	Image sprite;
	Image shadowSprite;
	Image colorSprite;
	int depth = 0;
	bool hFlip = false;
};

struct RenderOffsetComponent  {
	Vector2Int16 offset;
	Vector2Int16 shadowOffset;
};

struct RenderDestinationComponent  {
	Vector2Int16 dst;
	Vector2Int16 shadowDst;
};


struct NavigationWorkComponent {
	bool work;
};

struct NavigationComponent {
	Vector2Int16 target;
	uint8_t currentHeading;
	uint8_t targetHeading;
};

struct OrientationComponent {
	bool changed = false;
	uint8_t orientation = 0;
};

struct MovementComponent {
	uint8_t velocity;
	uint8_t rotationSpeed;

	void SetFromDef(const UnitDef& def) {
		velocity = def.MovementSpeed;
		rotationSpeed = def.RotationSpeed;
	}
};

struct UnitComponent {
	const UnitDef* def;
};

struct AnimationEnableComponent {
	bool pause = true;
};
struct AnimationTrackerComponent {
	int8_t clipFrame = 0;
	uint8_t totalFrames = 0;
	bool looping = false;

	inline void Restart() {
		clipFrame = -1;
	}

	inline void PlayClip(const AnimationClip* clip) {
		if (clip) {
			totalFrames = clip->GetFrameCount();
			looping = clip->looping;
			clipFrame = -1;
		}
	}
};

struct AnimationComponent {

	const AnimationClip* clip = nullptr;
	const AnimationClip* shadowClip = nullptr;
	const AnimationClip* unitColorClip = nullptr;

};