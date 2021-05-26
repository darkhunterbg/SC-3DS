#include "GameView.h"

#include "GameHUD.h"
#include "Cursor.h"
#include "../Game.h"
#include "../Camera.h"

#include "../Platform.h"
#include "../Util.h"
#include "../Entity/EntityManager.h"

#include "../Data/GraphicsDatabase.h"

#include "../Data/AbilityDatabase.h"

static std::array<Color, 3> GreyHPBarColorPalette = { 0xb0b0b0ff, 0x98948cff, 0x585858ff };
static std::array<Color, 3> YellowHPBarColorPalette = { 0xfccc2cff, 0xdc9434ff, 0xb09018ff };
static std::array<Color, 3> RedHPBarColorPalette = { 0xa80808ff, 0xa80808ff, 0x840404ff };
static std::array<Color, 3> GreenHPBarColorPalette = { 0x249824ff, 0x249824ff, 0x249824ff };

GameView::GameView(EntityManager& em, Vector2Int16 mapSizePixels)
{
	cursor = new Cursor();
	cursor->Position = { 200,120 };

	hud = new GameHUD(mapSizePixels);

	context.em = &em;
}

void GameView::SetPlayer(PlayerId player, const RaceDef& race)
{
	context.player = player;
	context.race = &race;
}

void GameView::Update(Camera& camera)
{
	for (int i = 0; i < context.selection.size(); ++i) {
		if (!context.IsVisibleUnit(context.selection[i])) {
			context.selection.RemoveEntity(context.selection[i]);
			--i;
		}
	}

	cursor->Update(camera, context);
	hud->Update(camera, context);
	UpdateMarkers();
	GamepadInput();


	auto& renderSystem = context.GetEntityManager().GetRenderSystem();
	renderSystem.ClearSelection();

	if (context.abilityTargetMarkerTimer > 0) {
		--context.abilityTargetMarkerTimer;

		if (context.IsAbilityTargetMarkerVisible()) {
			Color c = context.GetAlliedUnitColor(context.abilityTarget);
			renderSystem.AddSelection(context.abilityTarget, c);
		}
	}

	renderSystem.AddSelection(context.selection.GetEntities(), context.selectionColor);

}

void GameView::UpdateMarkers() {

	for (int i = 0; i < context.markers.size(); ++i) {
		context.markers[i].timer--;
		if (context.markers[i].timer == 0) {
			if (context.markers[i].state == 0) {
				++context.markers[i].state;
				context.markers[i].timer = GameViewContext::MarkerTimer;
			}
			else {
				context.markers.erase(context.markers.begin() + i);
				--i;
			}
		}
	}
}

void GameView::GamepadInput() {
	if (context.selection.size() == 0)
		return;

	if (Game::Gamepad.IsButtonReleased(GamepadButton::A)) {

		EntityId entity = cursor->GetEntityUnder();

		// TODO: diffentiate enemy/ally
		// TODO: goto unit position (follow state)
		if (entity == Entity::None) {
			if (context.IsTargetSelectionMode)
				context.ActivateCurrentAbility(cursor->GetWorldPosition());
			else
				context.ActivateAbility(&AbilityDatabase::Attack, cursor->GetWorldPosition());

			context.NewActionMarker(cursor->GetWorldPosition());
		}
		else {
			if (context.IsTargetSelectionMode)
				context.ActivateCurrentAbility(entity);
			else
				context.ActivateAbility(&AbilityDatabase::Attack, entity);

			context.NewUnitMarker(entity);
		}

		context.PlayUnitSelectedAudio(UnitChatType::Command);
	}

	if (Game::Gamepad.IsButtonReleased(GamepadButton::B)) {
		if (!context.IsTargetSelectionMode) {
			context.ActivateAbility(&AbilityDatabase::Stop);
		}
	}

	if (Game::Gamepad.IsButtonReleased(GamepadButton::X)) {
		EntityId entity = cursor->GetEntityUnder();

		if (entity == Entity::None) {
			if (!context.IsTargetSelectionMode)
			{
				context.ActivateAbility(&AbilityDatabase::Move, cursor->GetWorldPosition());
				context.NewActionMarker(cursor->GetWorldPosition());
				context.PlayUnitSelectedAudio(UnitChatType::Command);
			}
		}
		else {
			if (!context.IsTargetSelectionMode) {
				context.ActivateAbility(&AbilityDatabase::Move, cursor->GetWorldPosition());
				context.NewUnitMarker(entity);
				context.PlayUnitSelectedAudio(UnitChatType::Command);
			}
		}
	}
}

void GameView::DrawUpperScreen(const Camera& camera)
{
	DrawUnitBars(camera);
	DrawMarkers(camera);
	hud->UpperScreenGUI(camera, context);
	cursor->Draw();
}

void GameView::DrawLowerScreen(const Camera& camera)
{
	hud->LowerScreenGUI(camera, context);
}

void GameView::DrawMarkers(const Camera& camera) {
	Rectangle16 camRect = camera.GetRectangle16();

	int MarkerTimer = GameViewContext::MarkerTimer;

	for (GUIActionMarker& marker : context.markers) {

		if (!camRect.Contains(marker.pos))
			continue;

		const SpriteFrame& frame = GraphicsDatabase::Cursor.targg.GetFrame(1);

		Rectangle dst = { {0,0},  Vector2Int(frame.sprite.rect.size) };

		if (marker.state == 0)
		{
			float scale = 1;
			if (marker.timer < MarkerTimer / 2) {
				scale = (float)(marker.timer) / ((float)MarkerTimer);
			}
			else {
				scale = (float)(MarkerTimer - marker.timer) / ((float)MarkerTimer);

			}
			scale = 1 + scale;
			dst.size = Vector2Int(Vector2(dst.size) * scale);
		}


		Vector2Int16 p = camera.WorldToScreen(marker.pos);
		dst.SetCenter(Vector2Int(p));

		Platform::Draw(frame.sprite, dst);
	}

}

void GameView::DrawUnitBars(const Camera& camera) {
	Rectangle16 camRect = camera.GetRectangle16();
	EntityManager& em = context.GetEntityManager();

	for (EntityId id : context.selection) {

		Vector2Int16 pos = em.PositionComponents.GetComponent(id);
		pos.y += em.UnitArchetype.UnitComponents.GetComponent(id).def->Graphics->Selection.BarVerticalOffset;
		int barSize = em.UnitArchetype.UnitComponents.GetComponent(id).def->Graphics->Selection.BarSize;
		int size = barSize * 3 - 1;

		Rectangle16 bb = { {0,0}, Vector2Int16(size, 5) };
		bb.SetCenter(pos);

		if (!camRect.Intersects(bb))
			continue;

		UnitHealthComponent health = em.UnitArchetype.HealthComponents.GetComponent(id);

		auto* palette = &GreenHPBarColorPalette;

		int hpBarsVisible = (int)std::ceil(((float)health.current * barSize) / (float)health.max);

		if (hpBarsVisible > 1 && hpBarsVisible == barSize && !health.AtMax()) {
			--hpBarsVisible;
		}

		Vector2Int16 dst = camera.WorldToScreen(pos);
		dst.x -= size >> 1;

		if (hpBarsVisible != barSize) {

			auto* greyPalette = &GreyHPBarColorPalette;

			Rectangle greyDst = { {Vector2Int(dst) + Vector2Int(1,1) }, {size,1 } };

			for (int i = 0; i < 3; ++i) {
				Platform::DrawRectangle(greyDst, Color32(greyPalette->at(i)));
				++greyDst.position.y;
			}


			if (health.current * 100 < health.max * 33) {
				palette = &RedHPBarColorPalette;
			}

			else if (health.current * 100 < health.max * 66) {
				palette = &YellowHPBarColorPalette;
			}
		}

		Rectangle barHp = { Vector2Int(dst) + Vector2Int(1,1), Vector2Int(hpBarsVisible * 3 - 1 ,1) };

		for (int i = 0; i < 3; ++i) {
			Platform::DrawRectangle(barHp, Color32(palette->at(i)));
			++barHp.position.y;
		}

		Rectangle16 rect = { dst,Vector2Int16(size + 2,5) };
		Util::DrawTransparentRectangle(rect, 1, Colors::Black);

		Rectangle start = { Vector2Int(dst) + Vector2Int(3,1), Vector2Int(1,3) };

		for (int i = 1; i < barSize; ++i) {
			Platform::DrawRectangle(start, Color32(Colors::Black));
			start.position.x += 3;
		}
	}
}
