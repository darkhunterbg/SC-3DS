#include "GameView.h"

#include "GameHUD.h"
#include "Cursor.h"
#include "../Game.h"
#include "../Camera.h"

#include "../Util.h"
#include "../StringLib.h"
#include "../Entity/EntityUtil.h"
#include "../Entity/EntityManager.h"

#include "../Engine/GraphicsRenderer.h"
#include "../Engine/InputManager.h"

#include "../Data/AbilityDatabase.h"

static std::array<Color, 3> GreyHPBarColorPalette = { 0xb0b0b0ff, 0x98948cff, 0x585858ff };
static std::array<Color, 3> YellowHPBarColorPalette = { 0xfccc2cff, 0xdc9434ff, 0xb09018ff };
static std::array<Color, 3> RedHPBarColorPalette = { 0xa80808ff, 0xa80808ff, 0x840404ff };
static std::array<Color, 3> GreenHPBarColorPalette = { 0x249824ff, 0x249824ff, 0x249824ff };

static std::vector<PlayerEvent> newEvents;

static constexpr const int MessageTimer = 60 * 3;

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
	ContextualGamepadInput();

	if (context.GetEntityManager().GetPlayerSystem().NewEventsReady())
		ProcessEvents();

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

void GameView::ProcessEvents()
{
	newEvents.clear();

	context.GetEntityManager().GetPlayerSystem().GetPlayerEvents(context.player, PlayerEventType::All, newEvents);

	auto& raceDef = *context.race;

	for (const auto& ev : newEvents) {
		switch (ev.type)
		{
		case PlayerEventType::NewUnit: {
			context.GetEntityManager().GetSoundSystem().PlayUnitChat(ev.source, UnitChatType::Ready);
			break;
		}
		case PlayerEventType::NotEnoughSupply: {

			messageTimer = MessageTimer;
			stbsp_snprintf(message, sizeof(message), "Not enough %s...build more %s.", raceDef.SupplyNamePlural.data(), raceDef.SupplyBuildingNamePlural.data());

			context.GetEntityManager().GetSoundSystem().PlayAdviserErrorMessage(raceDef,
				AdvisorErrorMessageType::NotEnoughSupply);
			break;
		}
		case PlayerEventType::NotEnoughMinerals: {

			messageTimer = MessageTimer;
			stbsp_snprintf(message, sizeof(message), "Not enough minerals...mine more minerals.", raceDef.SupplyNamePlural.data(), raceDef.SupplyBuildingNamePlural.data());

			context.GetEntityManager().GetSoundSystem().PlayAdviserErrorMessage(raceDef,
				AdvisorErrorMessageType::NotEnoughMinerals);
			break;

			break;
		}
		default:
			break;
		}
	}
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

void GameView::ContextualGamepadInput() {
	if (context.selection.size() == 0)
		return;

	EntityId entity = cursor->GetEntityUnder();
	Vector2Int16 position = cursor->GetWorldPosition();
	bool commandTrigged = false;
	bool commandHasTarget = false;

	if (context.IsTargetSelectionMode) {
		if (InputManager::Gamepad.IsButtonReleased(GamepadButton::A)) {
			if (entity == Entity::None)
				context.ActivateCurrentAbility(position);
			else
				context.ActivateCurrentAbility(entity);

			commandTrigged = true;
			commandHasTarget = true;
		}
	}
	else {

		if (InputManager::Gamepad.IsButtonReleased(GamepadButton::A)) {

			if (entity == Entity::None) {
				context.ActivateAbility(&AbilityDatabase::Attack, position);
			}
			else {
				if (UnitEntityUtil::IsEnemy(context.player, entity))
					context.ActivateAbility(&AbilityDatabase::Attack, entity);
				else {
					if (context.GetEntityManager().UnitArchetype.DataComponents.GetComponent(entity).resources > 0)
					{
						context.ActivateAbility(&AbilityDatabase::Gather, entity);
					}
					else
						context.ActivateAbility(&AbilityDatabase::Move, entity);
				}
			}

			commandTrigged = true;
			commandHasTarget = true;
		}

		if (InputManager::Gamepad.IsButtonReleased(GamepadButton::B)) {

			EntityId id = context.GetPriorityUnitSelected();

			const auto& data = context.GetEntityManager().UnitArchetype.DataComponents.GetComponent(id);
			if (data.isBuilding) {
				if (!data.IsQueueEmpty()) {
					context.CancelBuildQueue(data.queueSize - 1);
				}
			}
			else {
				context.ActivateAbility(&AbilityDatabase::Stop);
				commandTrigged = true;
			}
		}

		if (InputManager::Gamepad.IsButtonReleased(GamepadButton::X)) {

			EntityId id = context.GetPriorityUnitSelected();

			const auto& data = context.GetEntityManager().UnitArchetype.DataComponents.GetComponent(id);
			if (data.isBuilding) {
				const auto& def = *context.GetEntityManager().UnitArchetype.UnitComponents.GetComponent(id).def;
				if (!data.IsQueueFull() && def.ProductionUnit != nullptr) {
					context.ActivateAbility(&AbilityDatabase::BuildUnit,
						*def.ProductionUnit);
				}
			}
			else {

				if (entity == Entity::None)
					context.ActivateAbility(&AbilityDatabase::Move, position);
				else
					context.ActivateAbility(&AbilityDatabase::Move, entity);

				commandTrigged = true;
				commandHasTarget = true;
			}
		}
	}

	if (commandTrigged) {
		context.PlayUnitSelectedAudio(UnitChatType::Command);

		if (commandHasTarget) {
			if (entity != Entity::None)
				context.NewUnitMarker(entity);
			else
				context.NewActionMarker(position);
		}
	}
}

void GameView::DrawUpperScreen(const Camera& camera)
{
	DrawUnitBars(camera);
	DrawMarkers(camera);
	hud->UpperScreenGUI(camera, context);
	cursor->Draw();

	if (messageTimer > 0) {
		--messageTimer;

		Vector2Int size = Game::SystemFont12->MeasureString( message);

		Vector2Int pos = { 0,190 };
		pos.x = (400 - size.x) / 2;

		GraphicsRenderer::DrawText(*Game::SystemFont12, pos + Vector2Int(1,1), message, Colors::Black);
		GraphicsRenderer::DrawText(*Game::SystemFont12, pos, message, Colors::White);
	}
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

		const SpriteFrame& frame = {};// GraphicsDatabase::Cursor.targg.GetFrame(1);

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

		GraphicsRenderer::Draw(frame.sprite, dst);
	}

}

void GameView::DrawUnitBars(const Camera& camera) {
	Rectangle16 camRect = camera.GetRectangle16();
	EntityManager& em = context.GetEntityManager();

	for (EntityId id : context.selection) {

		Vector2Int16 pos = em.PositionComponents.GetComponent(id);
		pos.y += em.UnitArchetype.UnitComponents.GetComponent(id).def->Graphics->Selection.BarVerticalOffset;
		int barSize = em.UnitArchetype.UnitComponents.GetComponent(id).def->Graphics->Selection.BarSize;

		barSize /= camera.Scale;

		int size = barSize * 3 - 1;

		Rectangle16 bb = { {0,0}, Vector2Int16(size, 5) };
		bb.SetCenter(pos);

		if (!camRect.Intersects(bb))
			continue;

		UnitHealthComponent health = em.UnitArchetype.HealthComponents.GetComponent(id);

		if (health.IsInvulnerable())
			continue;

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
				GraphicsRenderer::DrawRectangle(greyDst, Color32(greyPalette->at(i)));
				++greyDst.position.y;
			}


			if (health.current <= health.max / 3) {
				palette = &RedHPBarColorPalette;
			}

			else if (health.current <= (health.max << 1) / 3) {
				palette = &YellowHPBarColorPalette;
			}
		}

		Rectangle barHp = { Vector2Int(dst) + Vector2Int(1,1), Vector2Int(hpBarsVisible * 3 - 1 ,1) };

		for (int i = 0; i < 3; ++i) {
			GraphicsRenderer::DrawRectangle(barHp, Color32(palette->at(i)));
			++barHp.position.y;
		}

		Rectangle16 rect = { dst,Vector2Int16(size + 2,5) };
		Util::DrawTransparentRectangle(rect, 1, Colors::Black);

		Rectangle start = { Vector2Int(dst) + Vector2Int(3,1), Vector2Int(1,3) };

		for (int i = 1; i < barSize; ++i) {
			GraphicsRenderer::DrawRectangle(start, Color32(Colors::Black));
			start.position.x += 3;
		}
	}
}
