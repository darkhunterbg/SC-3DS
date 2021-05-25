#include "UnitSelectionConsolePanel.h"
#include "../Entity/EntityManager.h"

#include "../Platform.h"
#include "../Game.h"

#include "../Color.h"
#include "../StringLib.h"

static char buffer[64];

void UnitSelectionConsolePanel::Draw(Rectangle dst, const std::vector<EntityId>& selection, EntityManager& em)
{
	if (selection.size() == 0)
		return;

	Rectangle leftSpace = dst;
	leftSpace.size.x = 64;


	Rectangle rightSpace = dst;
	rightSpace.position.x += leftSpace.size.x;
	rightSpace.size.x -= leftSpace.size.x;

	EntityId entityId = selection.front();

	const UnitComponent& unit = em.UnitArchetype.UnitComponents.GetComponent(entityId);
	const UnitHealthComponent& health = em.UnitArchetype.HealthComponents.GetComponent(entityId);

	DrawUnitInfo(leftSpace, unit, health);
	DrawUnitDetail(rightSpace, unit);


}

void UnitSelectionConsolePanel::DrawUnitDetail(Rectangle space, const UnitComponent& unit)
{
	auto font = Game::SystemFont;

	Vector2Int pos = space.position + Vector2Int(space.size.x / 2, 0);

	int offset = Platform::MeasureString(font, unit.def->Name.data(), 0.4f).x;
	Platform::DrawText(font, pos - Vector2Int(offset / 2, 0), unit.def->Name.data(), Colors::UILightGray, 0.4f);
	pos.y += 18;

	if (unit.def->Title.length()) {
		offset = Platform::MeasureString(font, unit.def->Title.data(), 0.4f).x;
		Platform::DrawText(font, pos - Vector2Int(offset / 2, 0), unit.def->Title.data(), Colors::UILightGray, 0.4f);
	}

	pos.y += 14;

	stbsp_snprintf(buffer, sizeof(buffer), "Kills: %i", unit.kills);

	offset = 40;
	Platform::DrawText(font, pos - Vector2Int(offset / 2, 0), buffer, Colors::UILightGray, 0.4f);
	pos.y += 14;
}

void UnitSelectionConsolePanel::DrawUnitInfo(Rectangle space, const UnitComponent& unit, const UnitHealthComponent& health)
{
	auto font = Game::SystemFont;

	Rectangle wireframe = space;
	wireframe.size.y = 64;

	Rectangle stats = space;
	stats.position.y += wireframe.size.y;
	stats.size.y -= wireframe.size.y;

	Vector2Int pos = stats.position + Vector2Int{ stats.size.x / 2, 0};

	Color hpColor = Colors::UIGreen;

	if (health.current * 100 < health.max * 33) {
		hpColor = Colors::UIRed;
	}
	else if (health.current * 100 < health.max * 66) {
		hpColor = Colors::UIYellow;
	}

	int len = stbsp_snprintf(buffer, sizeof(buffer), "%i/%i", health.current, health.max);
	int offset = Platform::MeasureString(font, buffer, 0.35f).x;
	Platform::DrawText(font, pos - Vector2Int(offset / 2, 0), buffer, hpColor, 0.35f);

	// ================== Wireframe ========================

	static Color wfStateColor[3] = { Colors::UIGreen, Colors::UIYellow, Colors::UIRed };
	int wfPartsState[4] = { 0,0,0,0};
	
	int damgeBreakpoint = health.max / (4*2);

	int damageParts = 0;

	for (int i = health.current; i < health.max; i += damgeBreakpoint) {
		++damageParts;
	}

	for (int i = 0; i < damageParts; ++i) {
		++wfPartsState[i % 4];
	}

	const auto& wfBase = unit.def->Graphics->Wireframe.GetBase();

	Rectangle wfDst = wireframe;
	wfDst.position += Vector2Int(wfBase.offset);
	wfDst.size = Vector2Int(wfBase.sprite.rect.size);
	Platform::Draw(wfBase.sprite, wfDst);

	for (int i = 0; i < 4; ++i) {
		const auto& wfPart = unit.def->Graphics->Wireframe.GetPart(i);

		Rectangle wfDst = wireframe;
		wfDst.position += Vector2Int(wfPart.offset);
		wfDst.size = Vector2Int(wfPart.sprite.rect.size);
		Color c = wfStateColor[wfPartsState[i]];
		Platform::Draw(wfPart.sprite, wfDst, c);
	}
}
