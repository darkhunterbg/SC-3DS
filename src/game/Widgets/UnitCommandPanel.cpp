#include "UnitCommandPanel.h"
#include "../Data/RaceDef.h"
#include "../Data/GameDatabase.h"
#include "../GUI/GUI.h"
#include "../Color.h"
#include "../Game.h"
#include "../Entity/EntityUtil.h"
#include "../Entity/EntityManager.h"
#include "../StringLib.h"


void UnitCommandPanel::GenerateUnitCommands(EntityId id)
{
	auto& unitAbilities = EntityUtil::GetUnitAbilities(id);

	_commands.clear();

	const UnitAIComponent& ai = EntityUtil::GetManager().UnitSystem.GetAIComponent(id);

	for (const AbilityDef* ability : unitAbilities)
	{
		Command cmd = {};
		cmd.icon = &ability->Art.GetIcon();
		cmd.enabled = true;
		cmd.active = ability->IsState(ai.stateId);
		cmd.pos = ability->Art.ButtonPosition;
		cmd.text = ability->Art.Name;

		_commands.push_back(cmd);
	}

}

void UnitCommandPanel::Draw(EntityId id, const  RaceDef& skin)
{
	if (id == Entity::None) return;
	if (!EntityUtil::GetManager().UnitSystem.IsUnit(id)) return;

	GenerateUnitCommands(id);

	const auto& font = *Game::SystemFont8;
	//GUI::DrawLayoutDebug();

	const ImageFrame& normal = skin.CommandIcons->GetFrame(0);
	const ImageFrame& pressed = skin.CommandIcons->GetFrame(1);



	for (auto cmd : _commands)
	{
		Vector2Int pos = Vector2Int(cmd.pos);
		pos *= { 46, 40 };
		const ImageFrame& icon = *cmd.icon;

		Color c = cmd.enabled ? Colors::IconYellow : Colors::IconGray;
		if (cmd.active)
			c = Colors::White;

		GUI::BeginRelativeLayout(pos, Vector2Int(normal.size), GUIHAlign::Left, GUIVAlign::Top);

		Vector2Int btnOffset = { 0,0 };

		if (GUI::IsLayoutActivated())
		{
			c = Colors::White;
			btnOffset = { 1,1 };
		}

		GUIImage::DrawImageFrame(normal, btnOffset);
		GUI::BeginRelativeLayout(btnOffset, icon.GetRect().size);
		GUIImage::DrawImageFrame(icon, c);
		GUI::EndLayout();

		if (!String::IsEmpty(cmd.text))
		{
			if (GUI::IsLayoutHover())
				GUITooltip::DrawTextTooltip("CmdTooltip", font, cmd.text, TooltipLocation::Top);
		}

		GUI::EndLayout();
	}
}
