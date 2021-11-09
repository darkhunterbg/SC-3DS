#include "SelectionInfoPanel.h"
#include "../GUI/GUI.h"
#include "../Entity/EntityManager.h"
#include "../Data/GameDatabase.h"
#include "../Entity/EntityUtil.h"
#include "../Game.h"
#include "../StringLib.h"
#include "../Platform.h"

#include <array>

struct UnitItemInfo {
	const ImageFrame* sprite;
	uint8_t counter;
	const char* name = nullptr;
	const char* statName = nullptr;
	int stat = 0;
};

static void GetUnitWireframeColors(EntityId id, const UnitComponent& unit, std::array<Color, 6>& outColors)
{
	static Color wfStateColor[3] = { Colors::UIGreen, Colors::UIYellow, Colors::UIRed };
	int wfPartsState[4] = { 0,0,0,0 };

	int damgeBreakpoint = unit.maxHealth.value / (4 * 2);

	int damageParts = 0;


	for (int i = unit.health.value; i < unit.maxHealth.value; i += damgeBreakpoint)
	{
		++damageParts;
	}

	std::srand(id);

	for (int i = 0; i < damageParts; ++i)
	{

		int p = std::rand() % 4;

		bool found = false;

		for (p; p < 4; ++p)
		{
			if (wfPartsState[p] < 2)
			{
				found = true;
				++wfPartsState[p];
				break;
			}
		}

		if (!found)
		{
			for (p = 0; p < 4; ++p)
			{
				if (wfPartsState[p] < 2)
				{
					found = true;
					++wfPartsState[p];
					break;
				}
			}
		}
	}

	if (unit.HasShield())
	{
		int shieldState = ((unit.shield.value * 7) / unit.maxShield.value);
		/*	if (unit.shield.value > 0)
				++shieldState;*/

		outColors[0] = Colors::UIBlue;
		outColors[1] = Colors::UIBlue;

		outColors[0].a = 0;
		outColors[1].a = 0;


		if (shieldState > 6)
			shieldState = 6;

		if (shieldState > 3)
		{
			outColors[0].a = ((float)(shieldState - 4)) * 0.2f;
			outColors[0].a += 0.6f;
			shieldState = 3;
		}

		if (shieldState > 0)
		{
			outColors[1].a = ((float)(shieldState - 1)) * 0.2f;
			outColors[1].a += 0.6f;
		}


		//outColors[0].a = 1;
		//outColors[1].a = 1;

	}


	for (int i = 0; i < 4; ++i)
	{
		outColors[i + 2] = wfStateColor[wfPartsState[i]];
	}
}

void SelectionInfoPanel::Draw(std::vector<EntityId>& selection, const RaceDef& skin)
{
	if (selection.size() == 0) return;

	_raceDef = &skin;

	if (selection.size() > 1)
	{
		Vector2Int size = GUI::GetLayoutSpace().size;

		size -= {34, 18};

		GUI::BeginRelativeLayout({ 0,0 }, size, GUIHAlign::Center, GUIVAlign::Center);
		DrawMultiselection(selection);
		GUI::EndLayout(); return;
	}

	EntityId entityId = selection[0];

	Vector2Int size = GUI::GetLayoutSpace().size;
	size.x = 96;

	GUI::BeginRelativeLayout({ 0,0 }, size, GUIHAlign::Left);
	DrawUnitInfo(entityId);
	GUI::EndLayout();

	size.x = GUI::GetLayoutSpace().size.x - size.x;
	GUI::BeginRelativeLayout({ 0,0 }, size, GUIHAlign::Right, GUIVAlign::Top);
	DrawUnitName(entityId);
	GUI::EndLayout();

	size.y -= 32;
	GUI::BeginRelativeLayout({ 0,0 }, size, GUIHAlign::Right, GUIVAlign::Bottom);
	DrawUnitDetails(entityId);
	GUI::EndLayout();
}

void SelectionInfoPanel::DrawMultiselection(std::vector<EntityId>& selection)
{
	static constexpr const int Columns = 6;
	static constexpr const int Rows = 2;

	_temp.clear();
	_temp.insert(_temp.end(), selection.begin(), selection.end());

	const ImageFrame& f = _raceDef->CommandIcons->GetFrame(14);
	int max = std::min((int)_temp.size(), Columns * Rows);

	Vector2 space = Vector2(GUI::GetLayoutSpace().size);
	space.x -= f.size.x * Columns;
	space.x /= (Columns - 1);
	space.y -= f.size.y * Rows;
	space.y /= (Rows - 1);

	for (int i = 0; i < max; ++i)
	{
		Vector2Int offset = Vector2Int(Vector2(i / Rows, i % Rows) * (Vector2(f.size) + (space)));
		EntityId id = _temp[i];
		UnitComponent& unit = EntityUtil::GetManager().UnitSystem.GetComponent(id);

		GUI::BeginRelativeLayout(offset, Vector2Int(f.size), GUIHAlign::Left, GUIVAlign::Top);
		GUIImage::DrawImageFrame(f);

		std::array<Color, 6> wfColor;
		GetUnitWireframeColors(id, unit, wfColor);


		int wireframeStart = 0;
		if (!unit.HasShield())
			wireframeStart += 2;

		const auto& img = unit.def->Art.GetWireframe()->group.GetImage();
		int wireframeGraphicsOffset = unit.def->Art.GetWireframe()->HasShieldGraphics() ? 0 : -2;

		for (int i = wireframeStart; i < wfColor.size(); ++i)
		{
	
			const auto& wfPart = img.GetFrame(i + wireframeGraphicsOffset);

			GUI::AddNextElementOffset(Vector2Int(wfPart.offset) + Vector2Int(0, 1));
			GUI::SetNextElementSize(Vector2Int(wfPart.size));
			GUIImage::DrawImageFrame(wfPart, wfColor[i]);
		}


		if (GUI::IsLayoutActivated())
		{
			selection.clear();
			selection.push_back(id);
		}

		GUI::EndLayout();
	}
}

void SelectionInfoPanel::DrawUnitDetails(EntityId id)
{
	const auto& font = *Game::SystemFont10;

	UnitComponent& unit = EntityUtil::GetManager().UnitSystem.GetComponent(id);

	stbsp_snprintf(_buffer, sizeof(_buffer), "Kills: %i", unit.kills);

	GUILabel::DrawText(font, _buffer, GUIHAlign::Center, GUIVAlign::Top, Colors::UILightGray);

	// ======================== Upgrades ==========================

	const ImageFrame& f = _raceDef->CommandIcons->GetFrame(12);

	UnitItemInfo info[4];
	int infoCount = 0;

	if (unit.def->TechTree.GetArmorUpgrade() != nullptr)
	{
		info[infoCount].sprite = &unit.def->TechTree.GetArmorUpgrade()->GetIcon();
		info[infoCount].counter = 0;
		info[infoCount].name = unit.def->TechTree.GetArmorUpgrade()->Name;
		info[infoCount].statName = "Armor";
		info[infoCount].stat = unit.armor.IntValue();
		++infoCount;
	}

	if (unit.def->TechTree.GetShieldUpgrade() != nullptr)
	{
		info[infoCount].sprite = &unit.def->TechTree.GetShieldUpgrade()->GetIcon();
		info[infoCount].counter = 0;
		info[infoCount].name = unit.def->TechTree.GetShieldUpgrade()->Name;
		info[infoCount].statName = "Armor";
		info[infoCount].stat = 0;
		++infoCount;
	}


	int i = 0;
	for (const UnitAttack& atk : unit.def->GetAttacks())
	{
		info[infoCount].sprite = &atk.GetWeapon()->GetIcon();
		info[infoCount].counter = 0;
		info[infoCount].name = atk.GetWeapon()->Name;
		info[infoCount].statName = "Damage";
		info[infoCount].stat = unit.damage[i++].IntValue() * atk.GetWeapon()->Attacks;
		++infoCount;
	}

	Vector2Int position = { 0,0 };

	const auto& font2 = *Game::SystemFont8;

	for (int i = 0; i < infoCount; ++i)
	{
		GUI::BeginRelativeLayout(position, Vector2Int(f.size), GUIHAlign::Left, GUIVAlign::Bottom);

		GUI::AddNextElementOffset({ 2,2 });

		GUI::SetNextElementSize(Vector2Int(f.size) - Vector2Int{ 4, 4 });
		GUIImage::DrawImageFrame(*info[i].sprite, Colors::UILightGray);
		GUIImage::DrawImageFrame(f);

		stbsp_snprintf(_buffer, sizeof(_buffer), "%i", info[i].counter);
		GUI::BeginRelativeLayout({ -4,-2 }, { 12,10 }, GUIHAlign::Right, GUIVAlign::Bottom);
		GUILabel::DrawText(font2, _buffer, GUIHAlign::Center, GUIVAlign::Top, Colors::UILightGray);
		GUI::EndLayout();

		if (GUI::IsLayoutHover())
		{
			if (info[i].statName != nullptr)
				stbsp_snprintf(_buffer, sizeof(_buffer), "%s\n%s: %i", info[i].name, info[i].statName, info[i].stat);
			else
				stbsp_snprintf(_buffer, sizeof(_buffer), "%s", info[i].name);
			GUITooltip::DrawTextTooltip("Tooltip", font2, _buffer);
		}

		GUI::EndLayout();

		position.x += f.size.x + 4;
	}
}

void SelectionInfoPanel::DrawUnitName(EntityId id)
{
	const auto& font = *Game::SystemFont10;

	UnitComponent& unit = EntityUtil::GetManager().UnitSystem.GetComponent(id);

	GUILabel::DrawText(font, unit.def->Text.Name, GUIHAlign::Center, GUIVAlign::Top, Colors::UILightGray);

	if (unit.def->Text.HasTitle())
	{
		GUILabel::DrawText(font, unit.def->Text.Title, { 0,16 }, GUIHAlign::Center, GUIVAlign::Top, Colors::UILightGray);
	}
}

void SelectionInfoPanel::DrawUnitInfo(EntityId id)
{
	const auto& font = *Game::SystemFont8;
	UnitComponent& unit = EntityUtil::GetManager().UnitSystem.GetComponent(id);

	Color hpColor = Colors::UIGreen;

	if (unit.health <= (unit.maxHealth / 3))
	{
		hpColor = Colors::UIRed;
	}
	else if (unit.health <= (unit.maxHealth * 2) / 3)
	{
		hpColor = Colors::UIYellow;
	}



	int xOffset = 0;

	if (unit.HasShield())
	{
		stbsp_snprintf(_buffer, sizeof(_buffer), "%i/%i", unit.shield.IntValue(), unit.maxShield.IntValue());

		xOffset += Platform::MeasureString(font, _buffer).x;

		GUILabel::DrawText(font, _buffer, { -xOffset,64 }, GUIHAlign::Center, GUIVAlign::Top, Colors::UILightGray);
		xOffset /= 2;
	}

	stbsp_snprintf(_buffer, sizeof(_buffer), "%i/%i", unit.health.IntValue(), unit.maxHealth.IntValue());

	GUILabel::DrawText(font, _buffer, { xOffset,64 }, GUIHAlign::Center, GUIVAlign::Top, hpColor);

	// ================== Wireframe ========================

	std::array<Color, 6> wfColor;
	GetUnitWireframeColors(id, unit, wfColor);


	int wireframeStart = 0;
	if (!unit.HasShield() )
		wireframeStart += 2;

	int wireframeGraphicsOffset = unit.def->Art.GetWireframe()->HasShieldGraphics() ? 0 : -2;

	const auto& img = unit.def->Art.GetWireframe()->detail.GetImage();
	GUI::BeginRelativeLayout(Vector2Int{ 0,0 }, Vector2Int(img.GetSize()), GUIHAlign::Center, GUIVAlign::Top);
	for (int i = wireframeStart; i < wfColor.size(); ++i)
	{
		const auto& wfPart = img.GetFrame(i+ wireframeGraphicsOffset);

		GUI::AddNextElementOffset(Vector2Int(wfPart.offset));
		GUI::SetNextElementSize(Vector2Int(wfPart.size));
		GUIImage::DrawImageFrame(wfPart, wfColor[i]);
	}
	GUI::EndLayout();
}


