#pragma once

#include "../Assets.h"
#include <vector>
#include "../Entity/Entity.h"

class Camera;
class EntityManager;

class Cursor {
public:
	Cursor();
	Vector2Int Position = { 0,0 };
	Rectangle Limits = { {0,0},{400,240} };
private:
	const SpriteAtlas* atlas;
	const AnimationClip* currentClip;
	int clipFrame = 0;
	int clipCountdown = 0;
	Vector2Int holdStart = { 0,0 };
	Rectangle regionRect;
public:
	void Draw();
	void Update(Camera& camera, EntityManager& entityManager, std::vector<EntityId>& outSelection);
};