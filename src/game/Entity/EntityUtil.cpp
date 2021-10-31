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
	em.DrawSystem.GetComponent(id).color = em.PlayerSystem.GetPlayerInfo(owner).color;

	em.SetPosition(id, position);
	em.SetOrientation(id, orientation);
	em.UnitSystem.NewUnit(id, def, owner);

	if (def.Art.GetSprite().collider.size.LengthSquared() > 0)
		em.KinematicSystem.NewCollider(id, def.Art.GetSprite().collider);

	em.AnimationSystem.NewComponent(id);


	return id;
}


bool EntityUtil::IsAlly(PlayerId player, EntityId id)
{
	EntityManager& em = GetManager();
	return em.UnitSystem.GetComponent(id).owner == player;
}

bool EntityUtil::IsEnemy(PlayerId player, EntityId id)
{
	EntityManager& em = GetManager();
	auto owner = em.UnitSystem.GetComponent(id).owner;
	return owner.i != 0 && owner != player;
}

uint8_t EntityUtil::GetOrientationToPosition(EntityId id, Vector2Int16 target)
{
	EntityManager& em = GetManager();
	Vector2Int16 pos = em.GetPosition(id);
	//float dot = Vector2::Dot(Vector2(pos), Vector2(target));
	Vector2Int16 dir = target - pos;
	double angle = atan2(dir.y, dir.x);
	angle += PI;
	uint8_t result = (angle * 16.0 / PI);
	result = (result + 24) % 32;

	return result;
}

