#include "EntityUtil.h"

#include "EntityManager.h"

EntityManager* EntityUtil::emInstance = nullptr;

void EntityUtil::SetOrientation(EntityId id, int orientation)
{
	auto& em = GetManager();

	if (orientation < 0)
		orientation = 32 + (orientation % 32);
	else
		orientation %= 32;

	em.SetOrientation(id, orientation);

	UpdateAnimationVisual(id);
}

void EntityUtil::SetAnimationFrame(EntityId id, unsigned frame)
{
	auto& em = GetManager();

	AnimationComponent& anim = em.AnimationSystem.GetComponent(id);
	anim.animFrame = frame;

	UpdateAnimationVisual(id);
}

void EntityUtil::UpdateAnimationVisual(EntityId id)
{
	auto& em = GetManager();

	AnimationComponent& anim = em.AnimationSystem.GetComponent(id);
	uint8_t orientation = em.GetOrientation(id);
	bool flip = orientation > 16;
	unsigned frame = anim.animFrame;
	frame += flip ? 32 - orientation : orientation;

	if (anim.shadowImage == nullptr)
		em.DrawSystem.SetSpriteWithColor(id, *anim.baseImage, frame, flip);
	else
		em.DrawSystem.SetSpriteWithColorShadow(id, *anim.baseImage, *anim.shadowImage, frame, flip);
}

void EntityUtil::PlayAnimation(EntityId id, const AnimClipDef& clip, const Image* shadow)
{
	auto& em = GetManager();
	if (shadow == nullptr)
		em.AnimationSystem.StartAnimation(id, clip);
	else
		em.AnimationSystem.StartAnimationWithShadow(id, clip, *shadow);
	UpdateAnimationVisual(id);
}

EntityId EntityUtil::SpawnUnit(const UnitDef& def, PlayerId owner, Vector2Int16 position, int orientation)
{
	auto& em = GetManager();

	EntityId id = em.NewEntity();

	em.DrawSystem.NewComponent(id);
	em.DrawSystem.InitFromImage(id, def.Art.GetSprite().GetImage());
	em.SetPosition(id, position);
	em.SetOrientation(id, orientation);
	em.DrawSystem.GetComponent(id).color = em.PlayerSystem.GetPlayerInfo(owner).color;

	em.AnimationSystem.NewComponent(id);
	EntityUtil::PlayAnimation(id, *def.Art.GetSprite().GetAnimation(AnimationType::Init), def.Art.GetShadowImage());

	return id;
}
