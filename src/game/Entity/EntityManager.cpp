#include "EntityManager.h"
#include "RenderSystem.h"
#include "../Platform.h"
#include "../Profiler.h"
#include "../Job.h"

EntityManager::EntityManager() {
	renderSystem = new RenderSystem();
	animationSystem = new AnimationSystem();
	kinematicSystem = new KinematicSystem();
	navigationSystem = new NavigationSystem();
}
EntityManager::~EntityManager() {
	delete renderSystem;
	delete animationSystem;
	delete kinematicSystem;
	delete navigationSystem;
}

void EntityManager::DeleteEntity(EntityId id) {
	entities.DeleteEntity(id);
	
	// TODO: remove components;
}


void EntityManager::UpdateSecondaryEntities() {

	/*
	navigationArchetype.clear();

	int navSize = NavigationComponents.size();

	for (int i = 0; i < navSize; ++i) {
		if (NavigationWorkComponents[i].work) {
			navigationArchetype.movement.push_back(MovementComponents[i]);
			//navigationArchetype.position.push_back(PositionComponents[i]);
			navigationArchetype.navigation.push_back(&NavigationComponents[i]);
		}
	}

	navigationSystem->UpdateNavigation(navigationArchetype);
	*/
}


void EntityManager::UpdateEntities() {

	updated = true;

	/*
	movementArchetype.clear();

	int navSize = NavigationWorkComponents.size();

	for (int i = 0; i < navSize; ++i) {
		if (NavigationWorkComponents[i].work) {

			movementArchetype.work.push_back(&NavigationWorkComponents[i]);
			movementArchetype.movement.push_back(&MovementComponents[i]);
			//movementArchetype.position.push_back(&PositionComponents[i]);
			movementArchetype.navigation.push_back(NavigationComponents[i]);
			movementArchetype.changed.push_back(&EntityChangeComponents[i]);
			movementArchetype.unit.push_back(UnitComponents[i]);
			movementArchetype.anim.push_back(&AnimationComponents[i]);
			movementArchetype.animEnabled.push_back(&AnimationEnableComponents[i]);
			movementArchetype.animTracker.push_back(&AnimationTrackerComponents[i]);
		}
	}

	navigationSystem->MoveEntities(movementArchetype);

	//SectionProfiler p("Animations");

	animationSystem->GenerateAnimationUpdates(*this);

	animationSystem->UpdateAnimations();
	*/

	//p.Submit();

	renderSystem->UpdatePositions(*this);

}


void EntityManager::DrawEntites(const Camera& camera) {

	if (!updated)
		return;



	renderSystem->Draw(camera, *this);

	//kinematicSystem->DrawColliders(camera);


}

EntityId EntityManager::NewUnit(const UnitDef& def, Vector2Int position, Color color) {
	EntityId e = entities.NewEntity();
	PositionComponents.NewComponent(e, Vector2Int16( position));
	
	EntityChangeComponents.NewComponent(e, { true });

	RenderComponents.NewComponent(e, {
		Color4(color),
		def.MovementAnimations[0].GetFrame(0).sprite.image,
		def.MovementAnimationsShadow[0].GetFrame(0).sprite.image,
		def.MovementAnimationsTeamColor[0].GetFrame(0).sprite.image,
		});

	RenderOffsetComponents.NewComponent(e, {
	 Vector2Int16(def.MovementAnimations[0].GetFrame(0).offset),
		Vector2Int16(def.MovementAnimationsShadow[0].GetFrame(0).offset)
		});

	RenderDestinationComponents.NewComponent(e);
	RenderBoundingBoxComponents.NewComponent(e, { {0,0}, Vector2Int16( def.RenderSize) });

	RenderArchetype.AddEntity(e);

	/*
	NavigationComponents.NewComponent(e);
	NavigationWorkComponents.NewComponent(e, { false });
	MovementComponents.NewComponent(e, { 0,def.MovementSpeed, def.RotationSpeed });

	UnitComponents.NewComponent(e, { &def });

	auto& a = AnimationComponents.NewComponent(e);
	AnimationTrackerComponents.NewComponent(e);
	AnimationEnableComponents.NewComponent(e);
	*/

	return e;
}

void EntityManager::SetPosition(EntityId e, Vector2Int pos) {
	PositionComponents.GetComponent(e) = pos;
	EntityChangeComponents.GetComponent(e).changed = true;
}
void EntityManager::GoTo(EntityId e, Vector2Int pos) {

	NavigationWorkComponents.GetComponent(e).work = true;
	NavigationComponents.GetComponent(e).target = pos;
}

void EntityManager::SetOrientation(EntityId e, unsigned orientation)
{
	MovementComponents.GetComponent(e).orientation = orientation;
	auto& anim = AnimationComponents.GetComponent(e);
	auto& unit = UnitComponents.GetComponent(e);

	anim.clip = &unit.def->MovementAnimations[orientation];
	anim.shadowClip = &unit.def->MovementAnimationsShadow[orientation];
	anim.unitColorClip = &unit.def->MovementAnimationsTeamColor[orientation];
}

/*
RenderComponent& EntityManager::AddRenderComponent(EntityId id, const SpriteFrame& frame) {
	Entity& entity = GetEntity(id);
	entity.SetHasComponent<RenderComponent>(true);
	RenderComponent& c = renderSystem->RenderComponents.NewComponent(id);
	c.SetFrame(frame);
	c._dst += entity.position;
	c._shadowDst += entity.position;
	return c;
}
AnimationComponent& EntityManager::AddAnimationComponent(EntityId id, const AnimationClip* clip) {
	AnimationComponent& c = animationSystem->AnimationComponents.NewComponent(id);
	c.PlayClip(clip);
	Entity& entity = GetEntity(id);
	entity.SetHasComponent<AnimationComponent>(true);
	return c;
}
ColliderComponent& EntityManager::AddColliderComponent(EntityId id, const Rectangle& box) {
	Entity& entity = GetEntity(id);
	entity.SetHasComponent<ColliderComponent>(true);
	ColliderComponent& c = kinematicSystem->ColliderComponents.NewComponent(id);
	c.SetBox(box);
	c._worldBox.position += entity.position;
	return c;
}
NavigationComponent& EntityManager::AddNavigationComponent(EntityId id, int turnSpeed, int velocity) {
	Entity& entity = GetEntity(id);
	entity.SetHasComponent<NavigationComponent>(true);
	NavigationComponent& c = navigationSystem->NavigationComponents.NewComponent(id);
	c.turnSpeed = turnSpeed;
	c.velocity = velocity;
	return c;
}
*/