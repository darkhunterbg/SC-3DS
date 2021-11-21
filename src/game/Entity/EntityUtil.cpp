#include "EntityUtil.h"
#include "../Data/GameDatabase.h"
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


	em.KinematicSystem.NewKinematicComponent(id);

	em.AnimationSystem.NewComponent(id);
	em.MapSystem.SetVisionOccluded(id, true);

	PlayAnimation(id, *def.Art.GetSprite().GetAnimation(AnimationType::Init), def.Art.GetShadowImage());

	return id;
}

EntityId EntityUtil::SpawnSprite(const SpriteDef& def, Vector2Int16 position, int orientation)
{
	auto& em = GetManager();

	EntityId id = em.NewEntity();

	em.DrawSystem.NewComponent(id);
	em.DrawSystem.InitFromImage(id, def.GetImage());

	em.SetPosition(id, position);
	em.SetOrientation(id, orientation);

	em.AnimationSystem.NewComponent(id);
	em.MapSystem.SetVisionOccluded(id, true);

	PlayAnimation(id, *def.GetAnimation(AnimationType::Init));

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
	return !owner.IsNeutral() && owner != player;
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

void EntityUtil::SetUnitAIState(EntityId id, UnitAIStateId state)
{
	GetManager().UnitSystem.GetAIComponent(id).NewState(state);
}

void EntityUtil::SetUnitState(EntityId id, UnitStateId state)
{
	GetManager().UnitSystem.GetStateComponent(id).NewState(state);
}

static std::vector<const AbilityDef*> unitAbilities;

const std::vector<const AbilityDef*>& EntityUtil::GetUnitAbilities(EntityId id)
{
	unitAbilities.clear();

	if (id == Entity::None) return  unitAbilities;
	if (!GetManager().UnitSystem.IsUnit(id)) return unitAbilities;

	unitAbilities.push_back(GameDatabase::instance->StopAbility);
	unitAbilities.push_back(GameDatabase::instance->MoveAbility);
	unitAbilities.push_back(GameDatabase::instance->PatrolAbility);
	unitAbilities.push_back(GameDatabase::instance->HoldPositionAbility);

	const UnitComponent& unit = EntityUtil::GetManager().UnitSystem.GetComponent(id);

	if (unit.def->GetAttacks().Size() > 0)
		unitAbilities.push_back(GameDatabase::instance->AttackAbility);

	return unitAbilities;
}

bool EntityUtil::UnitHasAbility(EntityId id, const AbilityDef& ability)
{
	for (auto a : GetUnitAbilities(id)) {
		if (a && a->Id == ability.Id)
			return true;
	}
	return false;
}

void EntityUtil::ActivateAbility(EntityId user, const AbilityDef& ability)
{
	GAME_ASSERT(!ability.HasTargetSelection(), "Tried to activate ability '%s' without target!", ability.Art.Name);

	SetUnitAIState(user, ability.Data.EntitySelectedAction);

}

void EntityUtil::ActivateAbility(EntityId user, const AbilityDef& ability, EntityId target)
{
	auto& em = GetManager();

	GAME_ASSERT(ability.HasTargetSelection(), "Tried to activate ability '%s'  with target!", ability.Art.Name);

	if (ability.CanTargetEntity())
		em.UnitSystem.GetAIComponent(user).targetEntity = target;
	else
		em.UnitSystem.GetAIComponent(user).targetPosition = em.GetPosition(target);

	SetUnitAIState(user, ability.Data.EntitySelectedAction);
}



void EntityUtil::ActivateAbility(EntityId user, const AbilityDef& ability, Vector2Int16 target)
{
	GAME_ASSERT(ability.HasTargetSelection(), "Tried to activate ability '%s'  with target!", ability.Art.Name);

	auto& em = GetManager();

	auto& ai = em.UnitSystem.GetAIComponent(user);
	ai.targetPosition = target;
	ai.targetPosition2 = em.GetPosition(user);

	SetUnitAIState(user, ability.Data.PositionSelectedAction);
}

const AbilityDef* EntityUtil::GetUnitDefaultAbility(EntityId id, EntityId target)
{
	auto& em = GetManager();

	PlayerId owner = em.UnitSystem.GetComponent(id).owner;
	const UnitComponent& unit = em.UnitSystem.GetComponent(target);
	PlayerId targetOwner = unit.owner;

	if (targetOwner != owner && !targetOwner.IsNeutral())
	{
		if (unit.def->GetAttacks().Size() > 0)
			return GameDatabase::instance->AttackAbility;
	}

	return GameDatabase::instance->MoveAbility;

}

const AbilityDef* EntityUtil::GetUnitDefaultAbility(EntityId id, Vector2Int16 target)
{
	return GameDatabase::instance->MoveAbility;
}
