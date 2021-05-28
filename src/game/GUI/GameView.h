#pragma once

#include "GameViewContext.h"

#include "../Entity/Common.h"

class GameHUD;
class Cursor;
class Camera;

class GameView {

private:
	GameViewContext context;
	
	GameHUD* hud;
	Cursor* cursor;

	int messageTimer = 0;
	char message[128];

	void DrawMarkers(const Camera& camera);
	void UpdateMarkers();
	void DrawUnitBars(const Camera& camera);

	void ContextualGamepadInput();
public:
	GameView(EntityManager& em, Vector2Int16 mapSizePixels);

	void SetPlayer(PlayerId player, const RaceDef& race);

	void Update(Camera& camera);

	void ProcessEvents();

	void DrawUpperScreen(const Camera& camera);
	void DrawLowerScreen(const Camera& camera);

	inline GameViewContext& GetContext() {
		return context;
	}
};