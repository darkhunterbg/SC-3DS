#include "UnitSelectionConsolePanel.h"
#include "../Entity/EntityManager.h"
#include "../Entity/EntityUtil.h"

#include "../Game.h"

#include "../Color.h"
#include "../StringLib.h"

#include "../Engine/AssetLoader.h"
#include "../Engine/InputManager.h"
#include "../Engine/GraphicsRenderer.h"

static char buffer[64];

struct UnitItemInfo {

	const ImageFrame* sprite;
	uint8_t counter;
};

void UnitSelectionConsolePanel::Draw(GameViewContext& context)
{
	if (context.selection.size() == 0)
		return;

	if (context.selection.size() > 1) {

		DrawMultiSelection(PanelDst, context);
		return;
	}

	Rectangle leftSpace = PanelDst;
	leftSpace.size.x = 64;


	Rectangle rightSpace = PanelDst;
	rightSpace.position.x += leftSpace.size.x;
	rightSpace.size.x -= leftSpace.size.x;

	EntityId entityId = context.selection[0];

	auto& em = context.GetEntityManager();

	const UnitComponent& unit = em.UnitArchetype.UnitComponents.GetComponent(entityId);
	const UnitHealthComponent& health = em.UnitArchetype.HealthComponents.GetComponent(entityId);

	DrawUnitInfo(leftSpace, entityId, unit, health);

	Rectangle nameSpace = rightSpace;
	nameSpace.size.y = 30;

	DrawUnitName(nameSpace, entityId, unit, context);

	Rectangle detailSpace = rightSpace;
	detailSpace.position.y += nameSpace.size.y;
	detailSpace.size.y -= nameSpace.size.y;

	const UnitDataComponent& data = em.UnitArchetype.DataComponents.GetComponent(entityId);


	if (!data.IsQueueEmpty())
	{
		DrawProductionDetails(detailSpace, entityId, data, context);
	}
	else {
		/*
		if (unit.def->ProvideSupplyDoubled) {
			DrawSupplyInfo(detailSpace, entityId, unit, context);
		}
		else if (!unit.def->IsBuilding) {*/
		DrawUnitDetail(detailSpace, entityId, unit, context);
	}
	/*	else if (unit.def->IsResourceContainer) {


			if (data.resources > 0) {
				stbsp_snprintf(buffer, sizeof(buffer), "Minerals: %i", data.resources);

				const UnitDataComponent& data = context.GetEntityManager().UnitArchetype.DataComponents.GetComponent(entityId);

				Vector2Int off = detailSpace.position;
				off += { 30, 10};
				GraphicsRenderer::DrawText(*Game::SystemFont12, off, buffer, Colors::UILightGray);
			}
		}
		}*/

}

void UnitSelectionConsolePanel::UpdateSelection(GameViewContext& context)
{
	if (context.selection.size() < 2)
		return;

	if (!InputManager::Pointer.IsPressed())
		return;

	int max = std::min((int)context.selection.size(), 12);

	Vector2Int pos = PanelDst.position;
	pos.x += 3;
	pos.y += 6;

	for (int i = 0; i < max; ++i) {
		Vector2Int offset = Vector2Int(i / 2, i % 2) * 36;

		Rectangle rect = { { pos + offset}, {36,36} };

		if (rect.Contains(InputManager::Pointer.Position())) {

			if (context.IsTargetSelectionMode) {
				context.ActivateCurrentAbility(context.selection[i]);
			}
			else {
				EntityId entityId = context.selection[i];
				context.selection.clear();
				context.selection.AddEntity(entityId);

				context.GetEntityManager().GetSoundSystem().PlayUISound(*Game::ButtonAudio);
			}

			break;
		}
	}
}

void UnitSelectionConsolePanel::DrawMultiSelection(Rectangle dst, GameViewContext& context)
{
	const ImageFrame& f = context.GetCommandIcons().GetFrame(14);

	int max = std::min((int)context.selection.size(), 12);

	Vector2Int pos = dst.position;
	pos.x += 3;
	pos.y += 6;

	auto& em = context.GetEntityManager();

	for (int i = 0; i < max; ++i) {

		Vector2Int offset = Vector2Int(i / 2, i % 2) * 36;

		GraphicsRenderer::Draw(f, { pos + offset, Vector2Int(f.size) });

		EntityId entityId = context.selection[i];

		const UnitHealthComponent& health = em.UnitArchetype.HealthComponents.GetComponent(entityId);
		const UnitComponent& unit = em.UnitArchetype.UnitComponents.GetComponent(entityId);

		Color wfColor[4];
		GetUnitWireframeColors(entityId, health, wfColor);

		const auto& wfBase = ImageFrame();// unit.def->Graphics->Wireframe.GetGroupBase();

		offset += {1, 1};

		/*Rectangle wfDst = { pos + offset , {0,0} };
		wfDst.position += Vector2Int(wfBase.offset);
		wfDst.size = Vector2Int(wfBase.sprite.rect.size);
		Platform::Draw(wfBase.sprite, wfDst);*/

		for (int i = 0; i < 4; ++i) {
			const auto& wfPart = unit.def->Art.GetWireframe()->group.GetImage().GetFrame(i + 1);

			Rectangle wfDst = { pos + offset , {0,0} };
			wfDst.position += Vector2Int(wfPart.offset);
			wfDst.size = Vector2Int(wfPart.size);
			GraphicsRenderer::Draw(wfPart, wfDst, wfColor[i]);
		}
	}
}

void UnitSelectionConsolePanel::DrawUnitName(Rectangle space, EntityId id, const UnitComponent& unit, GameViewContext& context) {
	const auto& font = *Game::SystemFont12;

	Vector2Int pos = space.position + Vector2Int(space.size.x / 2, 0);

	int offset = font.MeasureString(unit.def->Text.Name).x;
	GraphicsRenderer::DrawText(font, pos - Vector2Int(offset / 2, 0), unit.def->Text.Name, Colors::UILightGray);
	pos.y += 16;

	if (unit.def->Text.HasTitle()) {
		offset = font.MeasureString(unit.def->Text.Title).x;
		GraphicsRenderer::DrawText(font, pos - Vector2Int(offset / 2, 0), unit.def->Text.Title, Colors::UILightGray);
	}
}

void UnitSelectionConsolePanel::DrawSupplyInfo(Rectangle space, EntityId id, const UnitComponent& unit, GameViewContext& context)
{
	if (!UnitEntityUtil::IsAlly(context.player, id))
		return;

	const auto& font = *Game::SystemFont10;

	Vector2Int pos = space.position;
	pos.x += 30;

	// TODO: RIGHT ALIGH

	const std::string& supplyName = context.GetPlayerRaceDef().SupplyNamePlural;

	const PlayerInfo& info = context.GetPlayerInfo();

	stbsp_snprintf(buffer, sizeof(buffer), "%s Used: %i", supplyName.data(), info.GetUsedSupply());
	int offset = 12;
	GraphicsRenderer::DrawText(font, pos + Vector2Int(offset, 0), buffer, Colors::UILightGray);
	pos.y += 12;

	//stbsp_snprintf(buffer, sizeof(buffer), "%s Provided: %i", supplyName.data(), unit.def->ProvideSupplyDoubled >> 1);
	offset = 0;
	GraphicsRenderer::DrawText(font, pos + Vector2Int(offset, 0), buffer, Colors::UILightGray);
	pos.y += 12;

	stbsp_snprintf(buffer, sizeof(buffer), "Total %s : %i", supplyName.data(), info.GetProvidedSupply());
	offset = 7;
	GraphicsRenderer::DrawText(font, pos + Vector2Int(offset, 0), buffer, Colors::UILightGray);
	pos.y += 12;

	stbsp_snprintf(buffer, sizeof(buffer), "%s Max : %i", supplyName.data(), info.GetMaxSupply());
	offset = 8;
	GraphicsRenderer::DrawText(font, pos + Vector2Int(offset, 0), buffer, Colors::UILightGray);
	pos.y += 12;
}

void UnitSelectionConsolePanel::DrawProductionDetails(Rectangle space, EntityId id, const UnitDataComponent& data, GameViewContext& context)
{
	if (!UnitEntityUtil::IsAlly(context.player, id))
		return;

	const auto& font = *Game::SystemFont10;

	Vector2Int pos = space.position;
	pos.x += 8;
	pos.y -= 16;

	const ImageFrame& f = context.GetCommandIcons().GetFrame(2);
	//const Sprite& f2 = context.GetCommandIconsAtlas().GetFrame(4).sprite;
	//const Sprite& a = context.GetCommandIconsAtlas().GetFrame(11).sprite;

	int total = data.queueSize;

	Rectangle dst;
	dst.position = pos;


	Vector2Int pointerPos = { 0,0 };
	if (InputManager::Pointer.IsPressed()) {
		pointerPos = InputManager::Pointer.Position();
	}

	for (int i = 0; i < data.productionQueue.size(); ++i) {

		dst.size = Vector2Int(f.size);
		GraphicsRenderer::Draw(f, dst);

		if (i < data.queueSize) {
			const ImageFrame& n = ImageFrame();// data.productionQueue[i]->Icon;
			Rectangle nDst = dst;
			nDst.size = Vector2Int(n.size);
			nDst.position += Vector2Int(n.offset) + Vector2Int{ 2,2 };

			GraphicsRenderer::Draw(n, nDst, Colors::IconYellow);

			stbsp_snprintf(buffer, sizeof(buffer), "%i", i + 1);

			Color c = i == 0 ? Colors::UILightGray : Colors::IconYellow;

			GraphicsRenderer::DrawText(font, nDst.position + Vector2Int{ 25,19 }, buffer, c);
		}
		else {
			const ImageFrame& n = context.GetCommandIcons().GetFrame(6 + i);

			Rectangle nDst = dst;

			nDst.size = Vector2Int(n.size);
			nDst.position += (dst.size - nDst.size) / 2;

			GraphicsRenderer::Draw(n, nDst);
		}

		if (dst.Contains(pointerPos)) {
			context.CancelBuildQueue(i);
		}

		if (i == 0 || i == data.productionQueue.size() - 1)
			dst.position += Vector2Int{ 0, dst.size.y };
		else
			dst.position += Vector2Int{ dst.size.x,0 };

	}


	GraphicsRenderer::DrawText(*Game::SystemFont12, pos + Vector2Int{ 68, 4 }, "Building", Colors::UILightGray);


	//const Sprite& pb = AssetLoader::LoadAtlas("game_gui.t3x")->GetSprite(0);

	//dst.position = pos + Vector2Int{ 38,24 };
	//dst.size = Vector2Int(pb.rect.size);
	//GraphicsRenderer::Draw(pb, dst);

	dst.position += {3, 3};
	dst.size = { 2,3 };

	int progress = 0;// data.productionQueue[0]->BuildTime - data.queueTimer;
	//progress = (progress * 100) / data.productionQueue[0]->BuildTime;

	if (progress == 0)
		return;

	for (int i = 0; i <= progress; i += 3) {

		GraphicsRenderer::DrawRectangle(dst, Color32(Colors::UIDarkGreen));
		dst.position.x += 3;
	}

	if (progress % 3 > 1) {
		dst.size.x = progress % 3 - 1;
		GraphicsRenderer::DrawRectangle(dst, Color32(Colors::UIDarkGreen));
	}
}

void UnitSelectionConsolePanel::DrawUnitDetail(Rectangle space, EntityId id, const UnitComponent& unit, GameViewContext& context)
{
	const auto& font = *Game::SystemFont12;

	Vector2Int pos = space.position + Vector2Int(space.size.x / 2, 0);


	stbsp_snprintf(buffer, sizeof(buffer), "Kills: %i", unit.kills);

	int offset = 40;
	GraphicsRenderer::DrawText(font, pos - Vector2Int(offset / 2, 0), buffer, Colors::UILightGray);
	pos.y += 18;


	// ======================== Upgrades ==========================

	pos.x = space.position.x;

	const ImageFrame& f = context.GetCommandIcons().GetFrame(12);


	UnitItemInfo info[4];
	int infoCount = 0;


	if (unit.def->TechTree.GetArmorUpgrade() != nullptr) {
		info[infoCount].sprite = &unit.def->TechTree.GetArmorUpgrade()->GetIcon();
		info[infoCount].counter = 0;
		++infoCount;
	}

	for (const UnitAttack& atk : unit.def->GetAttacks()) {
		info[infoCount].sprite = &atk.GetWeapon()->GetIcon();
		info[infoCount].counter = 0;
		++infoCount;
	}

	/*if (unit.def->Weapon) {
		info[infoCount].sprite = &unit.def->Weapon->Icon;
		info[infoCount].counter = 0;
		++infoCount;
	}*/

	for (int i = 0; i < infoCount; ++i) {
		Rectangle dst = { pos ,Vector2Int(f.size) };

		Rectangle icoDst = dst;
		icoDst.position += {2, 2};
		icoDst.size -= {4, 4};

		GraphicsRenderer::Draw(*info[i].sprite, icoDst, Colors::IconGray);
		GraphicsRenderer::Draw(f, dst);

		Vector2Int offset = { 24,23 };

		stbsp_snprintf(buffer, sizeof(buffer), "%i", info[i].counter);
		GraphicsRenderer::DrawText(*Game::SystemFont8, pos + offset, buffer, Colors::UILightGray);

		pos.x += 38;
	}
}

void UnitSelectionConsolePanel::DrawUnitInfo(Rectangle space, EntityId entityId, const UnitComponent& unit, const UnitHealthComponent& health)
{
	const auto& font = *Game::SystemFont10;

	Rectangle wireframe = space;
	wireframe.size.y = 64;


	if (!health.IsInvulnerable())
	{
		Rectangle stats = space;
		stats.position.y += wireframe.size.y;
		stats.size.y -= wireframe.size.y;

		Vector2Int pos = stats.position + Vector2Int{ stats.size.x / 2, 0 };

		Color hpColor = Colors::UIGreen;

		if (health.current <= (health.max / 3)) {
			hpColor = Colors::UIRed;
		}
		else if (health.current <= (health.max * 2) / 3) {
			hpColor = Colors::UIYellow;
		}

		int len = stbsp_snprintf(buffer, sizeof(buffer), "%i/%i", health.current, health.max);
		int offset = font.MeasureString(buffer).x;
		GraphicsRenderer::DrawText(font, pos - Vector2Int(offset / 2, 0), buffer, hpColor);

		/*pos.y += 10;

		len = stbsp_snprintf(buffer, sizeof(buffer), "%i/%i", 200, 250);
		offset = font.MeasureString(buffer).x;
		GraphicsRenderer::DrawText(font, pos - Vector2Int(offset / 2, 0), buffer, Colors::White);*/
	}

	// ================== Wireframe ========================

	//if (unit.def->Graphics->Wireframe.parts == 1) {
	//	const auto& wfPart = unit.def->Graphics->Wireframe.GetBase();
	//	Rectangle wfDst = wireframe;
	//	wfDst.position += Vector2Int(wfPart.offset);
	//	wfDst.size = Vector2Int(wfPart.sprite.rect.size);
	//	GraphicsRenderer::Draw(wfPart.sprite, wfDst, Colors::White);
	//}
	//else {

	Color wfColor[4];
	GetUnitWireframeColors(entityId, health, wfColor);

	//	const auto& wfBase = unit.def->Graphics->Wireframe.GetBase();

	//Rectangle wfDst = wireframe;
	//wfDst.position += Vector2Int(wfBase.offset);
	//wfDst.size = Vector2Int(wfBase.sprite.rect.size);
	//GraphicsRenderer::Draw(wfBase.sprite, wfDst);


	for (int i = 0; i < 4; ++i) {
		const auto& wfPart = unit.def->Art.GetWireframe()->detail.GetImage().GetFrame(i + 1);

		Rectangle wfDst = wireframe;
		wfDst.position += Vector2Int(wfPart.offset);
		wfDst.size = Vector2Int(wfPart.size);
		GraphicsRenderer::Draw(wfPart, wfDst, wfColor[i]);
	}
	//}
}

void UnitSelectionConsolePanel::GetUnitWireframeColors(EntityId id, const UnitHealthComponent& health, Color outColors[4])
{
	static Color wfStateColor[3] = { Colors::UIGreen, Colors::UIYellow, Colors::UIRed };
	int wfPartsState[4] = { 0,0,0,0 };

	int damgeBreakpoint = health.max / (4 * 2);

	int damageParts = 0;


	for (int i = health.current; i < health.max; i += damgeBreakpoint) {
		++damageParts;
	}

	std::srand(id);

	for (int i = 0; i < damageParts; ++i) {

		int p = std::rand() % 4;

		bool found = false;

		for (p; p < 4; ++p) {
			if (wfPartsState[p] < 2) {
				found = true;
				++wfPartsState[p];
				break;
			}
		}

		if (!found) {
			for (p = 0; p < 4; ++p) {
				if (wfPartsState[p] < 2) {
					found = true;
					++wfPartsState[p];
					break;
				}
			}
		}
	}

	for (int i = 0; i < 4; ++i) {
		outColors[i] = wfStateColor[wfPartsState[i]];
	}
}
