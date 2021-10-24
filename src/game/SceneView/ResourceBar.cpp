#include "ResourceBar.h"
#include "../GUI/GUI.h"
#include "../StringLib.h"
#include "../Game.h"
#include "../Data/GameDatabase.h"
#include "../MathLib.h"

static const int GetResourceUpdate(int change)
{
	int s = sign(change);
	change = std::abs(change);
	return std::max(1, (int)std::ceil(std::sqrt(change) / 1.5)) * s;
}


void ResourceBar::UpdateResourceDiff(ResourceBar::Resource& r)
{
	int diff = r.target - r.shown;
	if (diff == 0)
		return;

	int update = GetResourceUpdate(diff);

	int mod = std::abs(update);
	if (std::abs(diff) < mod)
		update = diff;
	r.shown += update;
}

void ResourceBar::DrawResource(Vector2Int offset, Resource& resource)
{

	auto font = Game::SystemFont12;

	Color color = Colors::UIGreen;

	stbsp_snprintf(_textBuffer, sizeof(_textBuffer), "%i", resource.shown);
	const ImageFrame& icon = *resource.icon;

	GUI::BeginRelativeLayout({ offset, Vector2Int(icon.size) }, GUIHAlign::Right, GUIVAlign::Top);
	GUIImage::DrawImageFrame(icon);
	GUILabel::DrawText(*font, _textBuffer, Vector2Int{ icon.size.x + 2,-2 },  color);
	GUI::EndLayout();

}
void ResourceBar::DrawSupply(Vector2Int offset, Supply& supply)
{
	auto font = Game::SystemFont12;
	const ImageFrame& icon = *supply.icon;

	Color color = Colors::UIGreen;
	if (supply.current > supply.max)
		color = Colors::UIRed;

	stbsp_snprintf(_textBuffer, sizeof(_textBuffer), "%i/%i", supply.current, supply.max);

	GUI::BeginRelativeLayout({ offset, Vector2Int(icon.size) }, GUIHAlign::Right, GUIVAlign::Top);
	GUIImage::DrawImageFrame(icon);
	GUILabel::DrawText(*font, _textBuffer, Vector2Int{ icon.size.x + 2,-2 }, color);
	GUI::EndLayout();
}

ResourceBar::ResourceBar()
{
	_minerals.icon = &GameDatabase::instance->GetImage("game\\icons\\min").GetFrame(0);

	for (int i = 0; i < 3; ++i)
	{
		RaceType r = (RaceType)(i + 1);
		_supply[i].icon = &GameDatabase::instance->GetRace(r)->SupplyIcon;
	}
}

void ResourceBar::UpdatePlayerInfo(const PlayerInfo& info, bool instant)
{
	_minerals.target = info.minerals;
	_gas.target = info.gas;

	for (int i = 0; i < 3; ++i)
	{
		_supply[i].enabled = false;
		_supply[i].current = _supply[i].max = 0;
	}

	int id = (int)info.race - 1;

	_supply[id].current = info.GetUsedSupply();
	_supply[id].max = info.GetProvidedSupply();
	_supply[id].enabled = true;

	if (instant)
	{
		_minerals.shown = _minerals.target;
		_gas.shown = _gas.target;
	}
	else
	{
		UpdateResourceDiff(_minerals);
		UpdateResourceDiff(_gas);
	}

}

void ResourceBar::Draw(const PlayerInfo& player)
{
	auto raceDef = GameDatabase::instance->GetRace(player.race);
	if (raceDef == nullptr)
		raceDef = GameDatabase::instance->GetRace(RaceType::Terran);

	int elements = 2;
	for (int i = 0; i < 3; ++i)
		if (_supply[i].enabled)
			elements++;

	_gas.icon = &raceDef->GasIcon;

	int elementSpace = GUI::GetLayoutSpace().size.x;
	elementSpace /= elements;
	elementSpace = std::min(80, elementSpace);

	Vector2Int offset = { -elementSpace * elements, 0};

	GUI::GetState().TextEffects.set(GUITextEffects_Shadow);

	DrawResource(offset, _minerals);
	offset.x += elementSpace;
	DrawResource(offset, _gas);

	for (int i = 0; i < 3; ++i)
	{
		if (!_supply[i].enabled) continue;
		offset.x += elementSpace;
		DrawSupply(offset, _supply[i]);
	}
	GUI::GetState().TextEffects.set(GUITextEffects_Shadow, false);
}
