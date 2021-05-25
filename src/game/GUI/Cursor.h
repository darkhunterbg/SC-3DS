#pragma once

#include "../Assets.h"
#include <vector>
#include "../Entity/Entity.h"
#include "../Entity/Common.h"

class Camera;
class EntityManager;

class Cursor {
public:
	Cursor();
	Vector2Int16 Position = { 0,0 };
	Rectangle16 Limits = { {0,0},{400,240} };
private:
	const SpriteAtlas* atlas;
	const AnimationClip* currentClip;
	int clipFrame = 0;
	int clipCountdown = 0;
	Vector2Int16 holdStart = { 0,0 };
	Rectangle16 regionRect;
public:
	PlayerId Player = 1;

	void Draw();
	void Update(Camera& camera, EntityManager& entityManager, std::vector<EntityId>& outSelection);
};