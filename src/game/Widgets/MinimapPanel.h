#pragma once

#include "MathLib.h"

class MinimapPanel {

public:
	struct ActivatedResult {
		Vector2Int16 worldPos;
		bool isAlternativeActivate = false;
		bool isActivate = false;

		inline bool Activated() const { return isActivate || isAlternativeActivate; }
	};

	bool PointerInputEnabled = true;


	ActivatedResult DrawMinimapAndAcitvate(class Camera& camera);
};