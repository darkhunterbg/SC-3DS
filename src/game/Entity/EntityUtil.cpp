#include "EntityUtil.h"
#include "EntityManager.h"
#include "../Data/GameDatabase.h"

EntityManager* EntityUtil::emInstance = nullptr;
EntityManager* UnitEntityUtil::emInstance = nullptr;


void EntityUtil::StartTimer(EntityId e, uint16_t time, TimerExpiredAction action, bool looping)
{
	EntityManager& em = GetManager();

	TimingComponent& t = em.TimingArchetype.TimingComponents.GetComponent(e);
	TimingActionComponent& a = em.TimingArchetype.ActionComponents.GetComponent(e);

	if (!em.TimingArchetype.Archetype.HasEntity(e)) {
		em.TimingArchetype.Archetype.AddEntity(e);
		// Don't need to reset components
		//em.TimingArchetype.TimingComponents.NewComponent(e);
		//em.TimingArchetype.ActionComponents.NewComponent(e);
	}

	t.NewTimer(time, looping);
	a.action = action;
	em.FlagComponents.GetComponent(e).set(ComponentFlags::UpdateTimers);
}

void EntityUtil::StartTimerMT(EntityId e, uint16_t time, TimerExpiredAction action, bool looping)
{
	EntityManager& em = GetManager();

	TimingComponent& t = em.TimingArchetype.TimingComponents.GetComponent(e);
	TimingActionComponent& a = em.TimingArchetype.ActionComponents.GetComponent(e);

	t.NewTimer(time, looping);
	a.action = action;
	em.FlagComponents.GetComponent(e).set(ComponentFlags::UpdateTimers);
}

void EntityUtil::SetPosition(EntityId e, Vector2Int16 pos) {
	EntityManager& em = GetManager();

	em.PositionComponents.GetComponent(e) = pos;
	em.FlagComponents.GetComponent(e).set(ComponentFlags::PositionChanged);
}


void EntityUtil::SetImageFrame(EntityId e, const Image& img, unsigned frameId, bool hFlip)
{
	EntityManager& em = GetManager();
	const auto& frame = img.GetFrame(frameId);

	em.RenderArchetype.BoundingBoxComponents.GetComponent(e).size = img.GetSize();
	em.RenderArchetype.OffsetComponents.GetComponent(e).offset = img.GetImageFrameOffset(frameId, hFlip);
	RenderComponent& render = em.RenderArchetype.RenderComponents.GetComponent(e);
	render.sprite = frame;
	render.hFlip = hFlip;
	render.colorMask = img.GetColorMaskFrame(frameId);
	em.FlagComponents.GetComponent(e).set(ComponentFlags::RenderChanged);
}

void EntityUtil::SetMapObjectBoundingBoxFromRender(EntityId e)
{
	EntityManager& em = GetManager();

	Rectangle16& mapBB = em.MapObjectArchetype.BoundingBoxComponents.GetComponent(e);
	const Rectangle16& renderBB = em.RenderArchetype.BoundingBoxComponents.GetComponent(e);

	Vector2Int16 size = renderBB.size;
	mapBB.size = Vector2Int16(size >> 6) << 1;
	mapBB.size = mapBB.size.Floor(1, 1);
	mapBB.position = (mapBB.size >> 1) * -1;
}

uint8_t EntityUtil::GetOrientationToPosition(EntityId id, Vector2Int16 target) {
	EntityManager& em = GetManager();
	Vector2Int16 pos = em.PositionComponents.GetComponent(id);
	//float dot = Vector2::Dot(Vector2(pos), Vector2(target));
	Vector2Int16 dir = target - pos;
	double angle = atan2(dir.y, dir.x);
	angle += PI;
	uint8_t result = (angle * 16.0 / PI);
	result = (result + 24) % 32;

	return result;
}

void EntityUtil::SetOrientation(EntityId id, int orientation)
{
	EntityManager& em = GetManager();

	if (orientation < 0)
		orientation = 32 + (orientation % 32);
	else
		orientation %= 32;

	em.OrientationComponents.GetComponent(id) = (uint8_t)orientation;
}

void EntityUtil::UpdateAnimationVisual(EntityId id)
{
	EntityManager& em = GetManager();
	const uint8_t& orientation = em.OrientationComponents.GetComponent(id);
	const auto& anim = em.AnimationArchetype.AnimationComponents.GetComponent(id);
	const auto& state = em.AnimationArchetype.StateComponents.GetComponent(id);

	unsigned frameId = state.animFrame;
	bool flip = orientation > 16;
	frameId += flip ? 32 - orientation : orientation;
	SetImageFrame(id, *anim.baseImage, frameId, flip);

	if (em.FlagComponents.GetComponent(id).test(ComponentFlags::RenderShadows)) {
		auto& offset = em.RenderArchetype.OffsetComponents.GetComponent(id);
		auto& shadow = em.RenderArchetype.ShadowComponents.GetComponent(id);
		const auto& img = em.AnimationArchetype.ShadowComponents.GetComponent(id);
		shadow.sprite = img.image->GetFrame(frameId);
		offset.shadowOffset = img.offset + img.image->GetImageFrameOffset(frameId, flip);
	}
}

void EntityUtil::PlayAnimation(EntityId id, const AnimClipDef& clip)
{
	EntityManager& em = GetManager();
	auto& anim = em.AnimationArchetype.AnimationComponents.GetComponent(id);
	const auto& sprite = GameDatabase::instance->SpriteDefs[clip.spriteId];
	anim.baseImage = &GameDatabase::instance->GetImage(sprite.imageId);
	anim.instructionStart = clip.instructionStart;
	anim.instructionEnd = clip.InstructionEnd();

	auto& state = em.AnimationArchetype.StateComponents.GetComponent(id);
	state.animFrame = 0;
	state.wait = 0;
	state.instructionId = anim.instructionStart;
}


void EntityUtil::CopyFlag(EntityId from, EntityId to, ComponentFlags flag) {
	EntityManager& em = GetManager();

	FlagsComponent& f = em.FlagComponents.GetComponent(from);
	em.FlagComponents.GetComponent(to).set(flag, f.test(flag));
}

EntityId UnitEntityUtil::NewUnit(const UnitDef& def, PlayerId playerId, Vector2Int16 position, EntityId e) {

	EntityManager& em = GetManager();

	const PlayerInfo& player = em.GetPlayerSystem().GetPlayerInfo(playerId);

	if (e == Entity::None)
		e = em.NewEntity();
	em.PositionComponents.NewComponent(e, position);
	em.OldPositionComponents.NewComponent(e, position);
	em.OrientationComponents.NewComponent(e, 0);

	em.FlagComponents.NewComponent(e);
	em.UnitArchetype.UnitComponents.NewComponent(e, { &def });
	em.UnitArchetype.HealthComponents.NewComponent(e).FromDef(def);

	em.UnitArchetype.MovementComponents.NewComponent(e).FromDef(def);
	em.UnitArchetype.DataComponents.NewComponent(e).FromDef(def);
	em.UnitArchetype.OwnerComponents.NewComponent(e, player.id);
	em.UnitArchetype.Archetype.AddEntity(e);

	em.UnitArchetype.PrevStateComponents.NewComponent(e);

	em.TimingArchetype.Archetype.AddEntity(e);


	em.RenderArchetype.Archetype.AddEntity(e);
	em.AnimationArchetype.Archetype.AddEntity(e);
	em.RenderArchetype.RenderComponents.GetComponent(e).color = player.color;
	auto& s = em.AnimationArchetype.ShadowComponents.GetComponent(e);
	s.image = def.Art.GetShadowImage();
	s.offset = def.Art.ShadowOffset;
	

	if (def.Art.GetSprite().animCount)
		EntityUtil::PlayAnimation(e, def.Art.GetSprite().GetClips()[0]);



	em.UnitArchetype.WeaponComponents.NewComponent(e).FromAttack(def.Attacks[0]);

	//if (def.MovementSpeed > 0) {
	//	em.NavigationArchetype.NavigationComponents.NewComponent(e);
	//	em.NavigationArchetype.Archetype.AddEntity(e);
	//}

	//em.CollisionArchetype.ColliderComponents.NewComponent(e).collider = def.Graphics->Collider;
	em.CollisionArchetype.Archetype.AddEntity(e);

	Rectangle16 mapBB;

	Vector2Int16 size = def.Art.GetSprite().GetImage().GetSize();
	mapBB.size = Vector2Int16(size >> 6) << 1;
	mapBB.size = mapBB.size.Floor(2, 2);
	mapBB.position -= mapBB.size / 2;

	//if (playerId == 0 && def.IsResourceContainer) {
	//	em.MapObjectArchetype.MinimapColorId.NewComponent(e, 15);
	//}
	//else {
	em.MapObjectArchetype.MinimapColorId.NewComponent(e, playerId);
	//}


	em.MapObjectArchetype.BoundingBoxComponents.NewComponent(e, mapBB);
	em.MapObjectArchetype.Archetype.AddEntity(e);


	//if (def.MovementSpeed > 0) {
	//	em.MovementArchetype.MovementComponents.NewComponent(e);
	//	em.MovementArchetype.Archetype.AddEntity(e);
	//}

	em.UnitArchetype.StateComponents.NewComponent(e);

	//if (def.IsBuilding) {
	//	em.FlagComponents.GetComponent(e).set(ComponentFlags::UnitAIPaused);
	//}
	//else {
	em.UnitArchetype.AIStateComponents.NewComponent(e, UnitAIState::Idle);
	em.UnitArchetype.AIStateDataComponents.NewComponent(e);
	//}

	em.SoundArchetype.Archetype.AddEntity(e);

	em.FlagComponents.GetComponent(e).set(ComponentFlags::PositionChanged);
	em.FlagComponents.GetComponent(e).set(ComponentFlags::RenderEnabled);
	em.FlagComponents.GetComponent(e).set(ComponentFlags::AnimationEnabled);
	em.FlagComponents.GetComponent(e).set(ComponentFlags::RenderShadows);
	em.FlagComponents.GetComponent(e).set(ComponentFlags::UnitStateChanged);
	em.FlagComponents.GetComponent(e).set(ComponentFlags::OrientationChanged);


	return e;
}

void UnitEntityUtil::AttackTarget(EntityId id, EntityId e)
{
	EntityManager& em = GetManager();

	em.UnitArchetype.StateDataComponents.GetComponent(id).target.entityId = e;
	em.UnitArchetype.StateComponents.GetComponent(id) = UnitState::Attacking;
	em.FlagComponents.GetComponent(id).set(ComponentFlags::UnitStateChanged);
	em.FlagComponents.GetComponent(id).clear(ComponentFlags::NavigationWork);
}

void UnitEntityUtil::GoTo(EntityId id, Vector2Int16 pos)
{
	EntityManager& em = GetManager();

	em.FlagComponents.GetComponent(id).set(ComponentFlags::NavigationWork);
	em.NavigationArchetype.NavigationComponents.GetComponent(id).target = pos;
	em.NavigationArchetype.NavigationComponents.GetComponent(id).targetHeading = 255;
}

void UnitEntityUtil::SetAIState(EntityId id, UnitAIState state)
{
	EntityManager& em = GetManager();
	em.FlagComponents.GetComponent(id).set(ComponentFlags::UnitAIStateChanged);
	em.UnitArchetype.AIStateComponents.GetComponent(id) = state;
}
void UnitEntityUtil::SetAIState(EntityId id, UnitAIState state, Vector2Int16 target)
{
	EntityManager& em = GetManager();
	em.FlagComponents.GetComponent(id).set(ComponentFlags::UnitAIStateChanged);
	em.UnitArchetype.AIStateComponents.GetComponent(id) = state;
	em.UnitArchetype.AIStateDataComponents.GetComponent(id).target.position = target;
}
void UnitEntityUtil::SetAIState(EntityId id, UnitAIState state, EntityId target)
{
	EntityManager& em = GetManager();
	em.FlagComponents.GetComponent(id).set(ComponentFlags::UnitAIStateChanged);
	em.UnitArchetype.AIStateComponents.GetComponent(id) = state;
	em.UnitArchetype.AIStateDataComponents.GetComponent(id).target.entityId = target;
}

bool UnitEntityUtil::IsAlly(PlayerId player, EntityId id)
{
	EntityManager& em = GetManager();
	return em.UnitArchetype.OwnerComponents.GetComponent(id) == player;
}

bool UnitEntityUtil::IsEnemy(PlayerId player, EntityId id)
{
	EntityManager& em = GetManager();
	auto owner = em.UnitArchetype.OwnerComponents.GetComponent(id);
	return owner != 0 && owner != player;
}

