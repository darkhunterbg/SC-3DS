#pragma once

#include "../Assets.h"

#include "Entity.h"
#include "IEntitySystem.h"
#include "EntityComponentMap.h"

class EntityManager;
struct AnimClipDef;
struct SpriteDef;

struct AnimationComponent {
	const Image* baseImage;
	const Image* shadowImage = nullptr;
	uint32_t instructionStart;
	uint32_t instructionEnd;

	int32_t instructionCounter = 0;
	uint16_t animFrame = 0;
	uint8_t wait;
	bool done = false;

	const AnimClipDef* clip = nullptr;
};

class AnimationSystem: public IEntitySystem {

	struct SpawnSprite {
		const SpriteDef* def;
		Vector2Int16 pos;
	};

	EntityComponentMap<AnimationComponent> _animComponents;


	std::vector<SpawnSprite> _spawnSprites;
	std::vector<EntityId> _destroy;
public:
	void RunAnimations(EntityManager& em);
	inline void NewComponent(EntityId id)
	{
		_animComponents.NewComponent(id);
	}

	inline AnimationComponent& GetComponent(EntityId id) { return _animComponents.GetComponent(id); }

	void StartAnimation(EntityId id, const AnimClipDef& clip);
	void StartAnimationWithShadow(EntityId id, const AnimClipDef& clip, const Image& shadow);

	// Inherited via IEntitySystem
	virtual void DeleteEntities(std::vector<EntityId>& entities) override;
	virtual size_t ReportMemoryUsage() override;

	void RegisterSpawnSprite(const SpriteDef& def, Vector2Int16 pos)
	{
		_spawnSprites.push_back({ &def ,pos });
	}
	void RegisterDestroy(EntityId id)
	{
		_destroy.push_back(id);
	}
};