#include "EntityUtil.h"
#include "EntityManager.h"

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
void EntityUtil::SetPosition(EntityId e, Vector2Int16 pos) {
	EntityManager& em = GetManager();

	em.PositionComponents.GetComponent(e) = pos;
	em.FlagComponents.GetComponent(e).set(ComponentFlags::PositionChanged);
}

void EntityUtil::CopyUnitRenderSettings(EntityId e) {
	EntityManager& em = GetManager();

	RenderComponent& render = em.RenderArchetype.RenderComponents.GetComponent(e);
	Vector2Int16& offset = em.RenderArchetype.OffsetComponents.GetComponent(e);
	Vector2Int16& dst = em.RenderArchetype.DestinationComponents.GetComponent(e);
	Rectangle16& bb = em.RenderArchetype.BoundingBoxComponents.GetComponent(e);

	const RenderUnitComponent& Urender = em.UnitArchetype.RenderArchetype.RenderComponents.GetComponent(e);
	const RenderUnitOffsetComponent& Uoffset = em.UnitArchetype.RenderArchetype.OffsetComponents.GetComponent(e);
	const RenderUnitDestinationComponent& Udst = em.UnitArchetype.RenderArchetype.DestinationComponents.GetComponent(e);
	const Rectangle16& Ubb = em.UnitArchetype.RenderArchetype.BoundingBoxComponents.GetComponent(e);

	render.sprite = Urender.sprite;
	render.depth = Urender.depth;
	render.hFlip = Urender.hFlip;

	offset = Uoffset.offset;
	dst = Udst.dst;
	bb = Ubb;
}

void EntityUtil::PlayAnimation(EntityId e, const AnimationClip& clip) {
	EntityManager& em = GetManager();

	AnimationComponent& a = em.AnimationArchetype.AnimationComponents.GetComponent(e);
	AnimationTrackerComponent& t = em.AnimationArchetype.TrackerComponents.GetComponent(e);
	FlagsComponent& f = em.FlagComponents.GetComponent(e);
	
	a.clip = &clip;
	t.PlayClip(&clip);
	f.set(ComponentFlags::AnimationEnabled);
	f.set(ComponentFlags::AnimationFrameChanged);

}

void EntityUtil::PlayAnimation(EntityId e, const UnitAnimationClip& clip) {
	EntityManager& em = GetManager();

	UnitAnimationComponent& a = em.UnitArchetype.AnimationArchetype.AnimationComponents.GetComponent(e);
	UnitAnimationTrackerComponent& t = em.UnitArchetype.AnimationArchetype.TrackerComponents.GetComponent(e);
	FlagsComponent& f = em.FlagComponents.GetComponent(e);

	a.clip = &clip;
	t.PlayClip(&clip);
	f.set(ComponentFlags::AnimationEnabled);
	f.set(ComponentFlags::AnimationFrameChanged);

}

void EntityUtil::SetRenderFromAnimationClip(EntityId e, const AnimationClip& clip, uint8_t i) {
	EntityManager& em = GetManager();

	RenderComponent& render = em.RenderArchetype.RenderComponents.GetComponent(e);
	Vector2Int16& offset = em.RenderArchetype.OffsetComponents.GetComponent(e);
	Rectangle16& bb = em.RenderArchetype.BoundingBoxComponents.GetComponent(e);
	FlagsComponent& f = em.FlagComponents.GetComponent(e);

	const SpriteFrame& frame = clip.GetFrame(i);
	render.SetSpriteFrame(frame);
	offset = frame.offset;
	bb.size = clip.GetFrameSize();
	
	f.set(ComponentFlags::RenderEnabled);
	f.set(ComponentFlags::RenderChanged);
}

void EntityUtil::SetRenderFromAnimationClip(EntityId e, const UnitAnimationClip& clip, uint8_t i) {
	EntityManager& em = GetManager();

	RenderUnitComponent& render = em.UnitArchetype.RenderArchetype.RenderComponents.GetComponent(e);
	RenderUnitOffsetComponent& offset = em.UnitArchetype.RenderArchetype.OffsetComponents.GetComponent(e);
	Rectangle16& bb = em.UnitArchetype.RenderArchetype.BoundingBoxComponents.GetComponent(e);
	FlagsComponent& f = em.FlagComponents.GetComponent(e);

	const UnitSpriteFrame& frame = clip.GetFrame(i);
	render.SetSpriteFrame(frame);
	offset.offset = frame.offset;
	offset.shadowOffset = frame.shadowOffset;
	bb.size = clip.GetFrameSize();

	f.set(ComponentFlags::RenderEnabled);
	f.set(ComponentFlags::RenderChanged);
}

EntityId UnitEntityUtil::NewUnit(const UnitDef& def, PlayerId playerId, Vector2Int16 position,  EntityId e) {

	EntityManager& em = GetManager();

	const PlayerInfo& player = em.GetPlayerSystem().GetPlayerInfo(playerId);

	if (e == Entity::None)
		e = em.NewEntity();
	em.PositionComponents.NewComponent(e, position);

	em.FlagComponents.NewComponent(e);
	em.UnitArchetype.UnitComponents.NewComponent(e, { &def });
	em.UnitArchetype.OrientationComponents.NewComponent(e);
	em.UnitArchetype.MovementComponents.NewComponent(e).FromDef(def);
	em.UnitArchetype.DataComponents.NewComponent(e).FromDef(def);
	em.UnitArchetype.OwnerComponents.NewComponent(e, player.id );
	em.UnitArchetype.Archetype.AddEntity(e);

	em.UnitArchetype.RenderArchetype.RenderComponents.NewComponent(e, {
		player.color,
		def.Graphics->MovementAnimations[0].GetFrame(0).sprite.image,
		def.Graphics->MovementAnimations[0].GetFrame(0).shadowSprite.image,
		def.Graphics->MovementAnimations[0].GetFrame(0).colorSprite.image,
		});

	em.UnitArchetype.RenderArchetype.OffsetComponents.NewComponent(e, {
	 Vector2Int16(def.Graphics->MovementAnimations[0].GetFrame(0).offset),
		Vector2Int16(def.Graphics->MovementAnimations[0].GetFrame(0).shadowOffset)
		});

	em.UnitArchetype.RenderArchetype.DestinationComponents.NewComponent(e);
	em.UnitArchetype.RenderArchetype.BoundingBoxComponents.NewComponent(e, { {0,0}, def.Graphics->RenderSize });

	em.UnitArchetype.RenderArchetype.Archetype.AddEntity(e);

	em.UnitArchetype.AnimationArchetype.AnimationComponents.NewComponent(e);
	em.UnitArchetype.AnimationArchetype.TrackerComponents.NewComponent(e).PlayClip(&def.Graphics->MovementAnimations[0]);

	em.UnitArchetype.AnimationArchetype.Archetype.AddEntity(e);

	em.NavigationArchetype.NavigationComponents.NewComponent(e);

	em.NavigationArchetype.Archetype.AddEntity(e);

	em.CollisionArchetype.ColliderComponents.NewComponent(e).collider = def.Graphics->Collider;
	em.CollisionArchetype.Archetype.AddEntity(e);

	em.MovementArchetype.MovementComponents.NewComponent(e);
	em.MovementArchetype.Archetype.AddEntity(e);

	em.FlagComponents.GetComponent(e).set(ComponentFlags::PositionChanged);
	em.FlagComponents.GetComponent(e).set(ComponentFlags::RenderEnabled);
	em.FlagComponents.GetComponent(e).set(ComponentFlags::RenderChanged);
	em.FlagComponents.GetComponent(e).set(ComponentFlags::AnimationFrameChanged);


	if (def.Graphics->HasMovementGlow()) {

		auto e2 = em.NewEntity();
		em.FlagComponents.NewComponent(e2);
		em.RenderArchetype.RenderComponents.GetComponent(e2).depth = 1;
		em.RenderArchetype.Archetype.AddEntity(e2);
		em.AnimationArchetype.Archetype.AddEntity(e2);

		em.ParentArchetype.Archetype.AddEntity(e);
		em.ParentArchetype.ChildComponents.NewComponent(e).AddChild(e2);

		em.UnitArchetype.UnitComponents.GetComponent(e).movementGlowEntity = e2;
	}

	return e;
}