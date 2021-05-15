#include "EntityUtil.h"
#include "EntityManager.h"

EntityManager* EntityUtil::emInstance = nullptr;

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