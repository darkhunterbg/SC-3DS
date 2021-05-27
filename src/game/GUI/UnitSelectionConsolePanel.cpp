#include "UnitSelectionConsolePanel.h"
#include "../Entity/EntityManager.h"

#include "../Platform.h"
#include "../Game.h"

#include "../Color.h"
#include "../StringLib.h"

static char buffer[64];

struct UnitItemInfo {

	const Sprite* sprite;
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

	if (unit.def->ProvideSupplyDoubled) {
		DrawSupplyInfo(detailSpace, entityId, unit, context);
	}
	else if (!unit.def->IsBuilding) {
		DrawUnitDetail(detailSpace, entityId, unit, context);
	}
}

void UnitSelectionConsolePanel::UpdateSelection(GameViewContext& context)
{
	if (context.selection.size() < 2)
		return;

	if (!Game::Pointer.IsPressed())
		return;

	int max = std::min((int)context.selection.size(), 12);

	Vector2Int pos = PanelDst.position;
	pos.x += 3;
	pos.y += 6;

	for (int i = 0; i < max; ++i) {
		Vector2Int offset = Vector2Int(i / 2, i % 2) * 36;

		Rectangle rect = { { pos + offset}, {36,36} };

		if (rect.Contains(Game::Pointer.Position())) {

			if (context.IsTargetSelectionMode) {
				context.ActivateCurrentAbility(context.selection[i]);
			}
			else {
				EntityId entityId = context.selection[i];
				context.selection.clear();
				context.selection.AddEntity(entityId);

				context.GetEntityManager().GetSoundSystem().PlayUISound(Game::ButtonAudio);
			}

			break;
		}
	}
}

void UnitSelectionConsolePanel::DrawMultiSelection(Rectangle dst, GameViewContext& context)
{
	const Sprite& f = context.GetCommandIconsAtlas().GetFrame(14).sprite;

	int max = std::min((int)context.selection.size(), 12);

	Vector2Int pos = dst.position;
	pos.x += 3;
	pos.y += 6;

	auto& em = context.GetEntityManager();

	for (int i = 0; i < max; ++i) {

		Vector2Int offset = Vector2Int(i / 2, i % 2) * 36;

		Platform::Draw(f, { pos + offset, Vector2Int(f.rect.size) });

		EntityId entityId = context.selection[i];

		const UnitHealthComponent& health = em.UnitArchetype.HealthComponents.GetComponent(entityId);
		const UnitComponent& unit = em.UnitArchetype.UnitComponents.GetComponent(entityId);

		Color wfColor[4];
		GetUnitWireframeColors(entityId, health, wfColor);

		const auto& wfBase = unit.def->Graphics->Wireframe.GetGroupBase();

		offset += {1, 1};

		/*Rectangle wfDst = { pos + offset , {0,0} };
		wfDst.position += Vector2Int(wfBase.offset);
		wfDst.size = Vector2Int(wfBase.sprite.rect.size);
		Platform::Draw(wfBase.sprite, wfDst);*/

		for (int i = 0; i < 4; ++i) {
			const auto& wfPart = unit.def->Graphics->Wireframe.GetGroupPart(i);

			Rectangle wfDst = { pos + offset , {0,0} };
			wfDst.position += Vector2Int(wfPart.offset);
			wfDst.size = Vector2Int(wfPart.sprite.rect.size);
			Platform::Draw(wfPart.sprite, wfDst, wfColor[i]);
		}
	}
}

void UnitSelectionConsolePanel::DrawUnitName(Rectangle space, EntityId id, const UnitComponent& unit, GameViewContext& context) {
	auto font = Game::SystemFont;

	Vector2Int pos = space.position + Vector2Int(space.size.x / 2, 0);

	int offset = Platform::MeasureString(font, unit.def->Name.data(), 0.4f).x;
	Platform::DrawText(font, pos - Vector2Int(offset / 2, 0), unit.def->Name.data(), Colors::UILightGray, 0.4f);
	pos.y += 16;

	if (unit.def->Title.length()) {
		offset = Platform::MeasureString(font, unit.def->Title.data(), 0.4f).x;
		Platform::DrawText(font, pos - Vector2Int(offset / 2, 0), unit.def->Title.data(), Colors::UILightGray, 0.4f);
	}
}

void UnitSelectionConsolePanel::DrawSupplyInfo(Rectangle space, EntityId id, const UnitComponent& unit, GameViewContext& context)
{
	auto font = Game::SystemFont;

	Vector2Int pos = space.position + Vector2Int(space.size.x, 0);
	pos.x -= 130;

	// TODO: RIGHT ALIGH

	const std::string& supplyName = context.GetPlayerRaceDef().SupplyNamePlural;

	const PlayerInfo& info = context.GetPlayerInfo();

	stbsp_snprintf(buffer, sizeof(buffer), "%s Used: %i", supplyName.data(), info.GetUsedSupply());
	int offset = 12;
	Platform::DrawText(font, pos + Vector2Int(offset, 0), buffer, Colors::UILightGray, 0.35f);
	pos.y += 12;

	stbsp_snprintf(buffer, sizeof(buffer), "%s Provided: %i", supplyName.data(), unit.def->ProvideSupplyDoubled >> 1);
	offset = 0;
	Platform::DrawText(font, pos + Vector2Int(offset, 0), buffer, Colors::UILightGray, 0.35f);
	pos.y += 12;

	stbsp_snprintf(buffer, sizeof(buffer), "Total %s : %i", supplyName.data(), info.GetProvidedSupply());
	offset = 7;
	Platform::DrawText(font, pos + Vector2Int(offset, 0), buffer, Colors::UILightGray, 0.35f);
	pos.y += 12;

	stbsp_snprintf(buffer, sizeof(buffer), "%s Max : %i", supplyName.data(), info.GetMaxSupply());
	offset = 8;
	Platform::DrawText(font, pos + Vector2Int(offset, 0), buffer, Colors::UILightGray, 0.35f);
	pos.y += 12;
}

void UnitSelectionConsolePanel::DrawUnitDetail(Rectangle space, EntityId id, const UnitComponent& unit, GameViewContext& context)
{
	auto font = Game::SystemFont;

	Vector2Int pos = space.position + Vector2Int(space.size.x / 2, 0);


	stbsp_snprintf(buffer, sizeof(buffer), "Kills: %i", unit.kills);

	int offset = 40;
	Platform::DrawText(font, pos - Vector2Int(offset / 2, 0), buffer, Colors::UILightGray, 0.4f);
	pos.y += 18;

	// ======================== Upgrades ==========================

	pos.x = space.position.x;

	const Sprite& f = context.GetCommandIconsAtlas().GetFrame(12).sprite;


	UnitItemInfo info[4];
	int infoCount = 0;


	if (unit.def->HasArmor()) {
		info[infoCount].sprite = &unit.def->ArmorIcon;
		info[infoCount].counter = 0;
		++infoCount;
	}

	if (unit.def->Weapon) {
		info[infoCount].sprite = &unit.def->Weapon->Icon;
		info[infoCount].counter = 0;
		++infoCount;
	}

	for (int i = 0; i < infoCount; ++i) {
		Rectangle dst = { pos ,Vector2Int(f.rect.size) };

		Rectangle icoDst = dst;
		icoDst.position += {2, 2};
		icoDst.size -= {4, 4};

		Platform::Draw(*info[i].sprite, icoDst, Colors::IconGray);
		Platform::Draw(f, dst);

		Vector2Int offset = { 24,23 };

		stbsp_snprintf(buffer, sizeof(buffer), "%i", info[i].counter);
		Platform::DrawText(font, pos + offset, buffer, Colors::UILightGray, 0.30f);

		pos.x += 38;
	}
}

void UnitSelectionConsolePanel::DrawUnitInfo(Rectangle space, EntityId entityId, const UnitComponent& unit, const UnitHealthComponent& health)
{
	auto font = Game::SystemFont;

	Rectangle wireframe = space;
	wireframe.size.y = 64;

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
	int offset = Platform::MeasureString(font, buffer, 0.35f).x;
	Platform::DrawText(font, pos - Vector2Int(offset / 2, 0), buffer, hpColor, 0.35f);

	//pos.y += 10;

	//len = stbsp_snprintf(buffer, sizeof(buffer), "%i/%i", 200, 250);
	// offset = Platform::MeasureString(font, buffer, 0.35f).x;
	//Platform::DrawText(font, pos - Vector2Int(offset / 2, 0), buffer, Colors::White, 0.35f);

	// ================== Wireframe ========================

	Color wfColor[4];
	GetUnitWireframeColors(entityId, health, wfColor);

	const auto& wfBase = unit.def->Graphics->Wireframe.GetBase();

	/*
	Rectangle wfDst = wireframe;
	wfDst.position += Vector2Int(wfBase.offset);
	wfDst.size = Vector2Int(wfBase.sprite.rect.size);
	Platform::Draw(wfBase.sprite, wfDst);
	*/

	for (int i = 0; i < 4; ++i) {
		const auto& wfPart = unit.def->Graphics->Wireframe.GetPart(i);

		Rectangle wfDst = wireframe;
		wfDst.position += Vector2Int(wfPart.offset);
		wfDst.size = Vector2Int(wfPart.sprite.rect.size);
		Platform::Draw(wfPart.sprite, wfDst, wfColor[i]);
	}
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
