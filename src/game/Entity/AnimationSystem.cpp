#include "AnimationSystem.h"
#include "RenderSystem.h"
#include <cstring>

AnimationSystem::AnimationSystem(int maxEntities) {
	entityToComponentMap = new int[maxEntities];
	memset(entityToComponentMap, -1, sizeof(maxEntities) * sizeof(int));
	this->maxComponents = maxEntities;
}
AnimationSystem::~AnimationSystem() {
	delete[] entityToComponentMap;
}

AnimationComponent& AnimationSystem::NewComponent(EntityId id, const AnimationClip* clip) {

	int cid = animationComponents.size();
	entityToComponentMap[id - 1] = cid;
	componentToEntityMap.push_back(id);
	animationComponents.push_back(AnimationComponent());
	AnimationComponent& c = animationComponents[cid];
	c = AnimationComponent();
	c.PlayClip(clip);

	return c;
}
void AnimationSystem::RemoveComponent(EntityId id) {
	int cid = entityToComponentMap[id - 1];
	entityToComponentMap[id - 1] = 0;
	animationComponents.erase(animationComponents.begin() + cid);
	componentToEntityMap.erase(componentToEntityMap.begin() + cid);

	int size = componentToEntityMap.size();
	for (int i = cid; i < size; ++i) {
		EntityId eid = componentToEntityMap[i];
		entityToComponentMap[eid - 1] -= 1;
	}
}


void AnimationSystem::UpdateAnimations(RenderSystem& renderSystem) {
	animationUpdates.clear();
	int cid = 0;
	for (AnimationComponent& cmp : animationComponents) {
		cmp.frameCountdown--;
		if (cmp.frameCountdown == 0) {
			cmp.clipFrame++;
	
			cmp.frameCountdown = cmp.clip->frameDuration;
			int framesCount = cmp.clip->GetSpriteCount();
			if (cmp.clip->looping)
				cmp.clipFrame %= framesCount;

			if (cmp.clipFrame < framesCount)
			{
				EntityId eid = componentToEntityMap[cid];
				AnimationComponent& cmp = animationComponents[cid];
				RenderComponent* rcomp = renderSystem.GetComponent(eid);
				rcomp->sprite = cmp.clip->GetSprite(cmp.clipFrame);
				rcomp->_dst.size = rcomp->sprite.rect.size;
			}
		}
		++cid;
	}

}
