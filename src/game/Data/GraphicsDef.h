#pragma once

#include "AnimationDef.h"
#include "Assets.h"
#include "../MathLib.h"
#include <functional>

struct SpriteAtlasDef {
	const char* Path = nullptr;
	const SpriteAtlas* Atlas = nullptr;

	inline const Sprite& GetSprite(unsigned index) const {
		return Atlas->GetSprite(index);
	}

	SpriteAtlasDef() {}
	SpriteAtlasDef(const SpriteAtlasDef&) = delete;
	SpriteAtlasDef& operator=(const SpriteAtlasDef&) = delete;

	void Load();
};

struct UnitGraphicsDef {
	Vector2Int16 RenderSize;
	Rectangle16 Collider;

	UnitDirectionalAnimationDef MovementAnimationDef;
	UnitDirectionalAnimationDef IdleAnimationDef;
	UnitDirectionalAnimationDef AttackAnimationDef;
	UnitAnimationDef DeathAnimationDef;

	DirectionalAnimationDef MovementGlowAnimationDef;

	UnitAnimationClip MovementAnimations[32];
	AnimationClip MovementGlowAnimations[32];
	UnitAnimationClip AttackAnimations[32];
	UnitAnimationClip IdleAnimations[32];
	UnitAnimationClip DeathAnimation;

	struct {
		const SpriteFrameAtlas* Atlas = nullptr;
		short VecticalOffset = 0;
		short BarSize = 1;
		short BarVerticalOffset = 0;
	} Selection;

	struct {
		AnimationDef Def;
		AnimationClip Clip;
		int Depth = 0;

	} DeathAfterEffect;

	struct {
		const SpriteFrameAtlas* Atlas = nullptr;
		int PartsCount = 4;

		inline const SpriteFrame& GetBase() const { return Atlas->GetFrame(0); }
		inline const SpriteFrame& GetPart(int i) const { return Atlas->GetFrame(i + 1); }
	} Wireframe;

	std::function<void()> LoadResourcesAction;

	inline bool HasMovementGlow() const { return MovementGlowAnimationDef.FrameCount > 0; }
	inline bool HasDeathAfterEffect() const { return DeathAfterEffect.Def.FrameCount > 0; };
	inline bool HasDeathAnimation() const { return DeathAnimationDef.FrameCount > 0; }

	UnitGraphicsDef() {}
	UnitGraphicsDef(const UnitGraphicsDef&) = delete;
	UnitGraphicsDef& operator=(const UnitGraphicsDef&) = delete;
};