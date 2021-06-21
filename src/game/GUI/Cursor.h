#pragma once

#include "../Assets.h"
#include <vector>
#include "../Entity/Entity.h"
#include "../Entity/Common.h"
#include "GUIAnimator.h"

class Camera;
class GameViewContext;

class Cursor {
public:
	Cursor();
	Vector2Int16 Position = { 0,0 };
	Rectangle16 Limits = { {0,0},{400,240} };
private:
	GUIAnimator animator;

	std::string currentClip;

	Vector2Int16 holdStart = { 0,0 };
	Rectangle16 regionRect;
	Vector2Int corner = { 0,0 };
	bool holding, dragging;
	EntityId hover;
	Vector2Int16 worldPos;

	std::string newClip;

	void UpdateDefaultState(Camera& camera, GameViewContext& context);
	void UpdateTargetSelectionState(Camera& camera, GameViewContext& context);
public:
	void Draw();
	void Update(Camera& camera, GameViewContext& context);

	inline EntityId GetEntityUnder() const { return hover; }
	inline Vector2Int16 GetWorldPosition() const {
		return worldPos;
	}
};