#include "UnitCommandPanel.h"
#include "../Data/RaceDef.h"
#include "../Data/GameDatabase.h"
#include "../GUI/GUI.h"
#include "../Color.h"
#include "../Game.h"
#include "../Entity/EntityUtil.h"
#include "../Entity/EntityManager.h"
#include "../StringLib.h"

static constexpr const int CommandDelay = 1;

const UnitCommandPanel::Command* UnitCommandPanel::DrawUnitCommandsAndSelect(EntityId id, const  RaceDef& skin)
{
	if (id == Entity::None) return nullptr;
	if (!EntityUtil::GetManager().UnitSystem.IsUnit(id)) return nullptr;

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
		cmd.ability = ability;

		_commands.push_back(cmd);
	}

	return DrawCommandButtonsAndSelect(skin);
}

const UnitCommandPanel::Command* UnitCommandPanel::DrawCommandButtonsAndSelect(const RaceDef& skin)
{
	const auto& font = *Game::SystemFont8;
	//GUI::DrawLayoutDebug();

	const ImageFrame& normal = skin.CommandIcons->GetFrame(0);
	const ImageFrame& pressed = skin.CommandIcons->GetFrame(1);

	for (Command& cmd : _commands)
	{
		Vector2Int pos = Vector2Int(cmd.pos);
		pos *= { 46, 40 };
		const ImageFrame& icon = *cmd.icon;

		Color c = cmd.enabled ? Colors::IconYellow : Colors::IconGray;
		if (cmd.active)
			c = Colors::White;

		GUI::BeginRelativeLayout(pos, Vector2Int(normal.size), GUIHAlign::Left, GUIVAlign::Top);

		Vector2Int btnOffset = { 0,0 };

		if (GUI::IsLayoutPressed())
		{
			c = Colors::White;
			btnOffset = { 1,1 };

		}

		if (GUI::IsLayoutActivated())
		{
			_activatedCmd = cmd;
			_commandActivateDelay = CommandDelay + 1;	// +1 because later we reduce value
		}

		GUIImage::DrawImageFrame(normal, btnOffset);
		GUI::BeginRelativeLayout(btnOffset, icon.GetRect().size);
		GUIImage::DrawImageFrame(icon, c);
		GUI::EndLayout();

		if (!String::IsEmpty(cmd.text))
		{
			if (GUI::IsLayoutFocused())
				GUITooltip::DrawTextTooltip("CmdTooltip", font, cmd.text, TooltipLocation::Top);
		}

		GUI::EndLayout();
	}

	if (_commandActivateDelay >0 )
	{
		--_commandActivateDelay;
		if (_commandActivateDelay == 0)
			return &_activatedCmd;
	}
	return nullptr;
}

const UnitCommandPanel::Command* UnitCommandPanel::DrawAbilityCommandsAndSelect(const AbilityDef* ability, const RaceDef& skin)
{
	_commands.clear();

	Command cmd = {};
	cmd.icon = &GameDatabase::instance->GetIcon(236);
	cmd.enabled = true;
	cmd.active = false;
	cmd.pos = { 2,2 };
	cmd.text = "Cancel";

	_commands.push_back(cmd);

	return DrawCommandButtonsAndSelect(skin);
}
