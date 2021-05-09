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

#include "../Platform.h"

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
	Vector2Int target;
	unsigned targetHeading;
};

struct MovementComponent {
	unsigned orientation;
	unsigned velocity;
	unsigned rotationSpeed;
};

struct UnitComponent {
	const UnitDef* def;
};

struct AnimationEnableComponent {
	bool pause = true;
};
struct AnimationTrackerComponent {
	int clipFrame = 0;
	int frameCountdown = 0;
	int totalFrames = 0;
	int frameDuration = 0;
	bool looping = false;

	inline void Restart() {
		clipFrame = -1;
		frameCountdown = 1;
	}

	inline void PlayClip(const AnimationClip* clip) {
		if (clip) {
			totalFrames = clip->GetFrameCount();
			frameDuration = clip->frameDuration;
			looping = clip->looping;
			clipFrame = -1;
			frameCountdown = 1;
		}
	}
};

struct AnimationComponent {

	const AnimationClip* clip = nullptr;
	const AnimationClip* shadowClip = nullptr;
	const AnimationClip* unitColorClip = nullptr;

};