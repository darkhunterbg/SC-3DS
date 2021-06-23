#include "UnitCommandsPanel.h"
#include "../Color.h"
#include "../Game.h"

#include "../Entity/EntityManager.h"
#include "../Entity/Common.h"
#include "../Entity/EntityUtil.h"

#include "../Engine/GraphicsRenderer.h"
#include "../Engine/InputManager.h"

#include "../Data/GameDatabase.h"


static std::vector<const AbilityDef*> unitAbilities;

void UnitCommandsPanel::Draw(GameViewContext& context)
{
	DrawCommands(context);
}

void UnitCommandsPanel::UpdateInput(GameViewContext& context)
{
	UpdateCommands(context);

	if (context.IsTargetSelectionMode &&
		InputManager::Gamepad.IsButtonReleased(GamepadButton::B)) {

		OnCommandPressed(context, unitCommands[8]);
	}

	Rectangle dst = PanelDst;

	const ImageFrame& f = context.GetCommandIcons().GetFrame(0);

	dst.size = Vector2Int(f.size);

	if (InputManager::Pointer.IsDown()) {

		hover = -1;

		for (int i = 0; i < 9; ++i) {
			int x = i % 3;
			int y = i / 3;
			const UnitCommand& cmd = unitCommands[i];

			if (!cmd.IsUsable())
				continue;

			Vector2Int offset = { x * 46, y * 40 };
			Rectangle d = dst;
			d.position += offset;

			if (d.Contains(InputManager::Pointer.Position())) {
				hover = i;
				break;
			}

		}

		if (InputManager::Pointer.IsPressed() && hover != -1)
		{
			pressedCommand = hover;
		}
	}


	if (pressedCommand != -1) {
		unitCommands[pressedCommand].pressed = true;
		unitCommands[pressedCommand].active = true;
	}

	if (pressedCommand != -1 && InputManager::Pointer.IsReleased()) {
		if (pressedCommand == hover) {
			OnCommandPressed(context, unitCommands[pressedCommand]);
		}

		pressedCommand = -1;
	}
}

void UnitCommandsPanel::DrawCommands(GameViewContext& context) {

	Rectangle dst = PanelDst;

	const ImageFrame& normal = context.GetCommandIcons().GetFrame(0);
	const ImageFrame& pressed = context.GetCommandIcons().GetFrame(1);

	for (int y = 0; y < 3; y++) {
		for (int x = 0; x < 3; ++x) {

			const UnitCommand& cmd = unitCommands[x + y * 3];

			if (!cmd.IsVisible())
				continue;

			Vector2Int offset = { x * 46, y * 40 };


			const ImageFrame& f = cmd.pressed ? pressed : normal;
			dst.size = Vector2Int(f.size);

			Rectangle d = dst;
			d.position += offset;
			GraphicsRenderer::Draw(f, d);

			const ImageFrame& commandIcon = cmd.commandIcon != nullptr ?
				*cmd.commandIcon : cmd.ability->Art.GetIcon();

			d.size = Vector2Int(commandIcon.size);
			d.position = dst.position + offset +
				(Vector2Int{ 36, 35 } - Vector2Int(commandIcon.offset + commandIcon.size)) / 2;


			if (cmd.pressed) {
				// TODO: sprite frames are needed here
				d.position += {1, 1};
			}

			Color color = Colors::IconGray;

			if (cmd.enabled)
				color = Colors::IconYellow;

			if (cmd.active)
				color = Colors::White;

			GraphicsRenderer::Draw(commandIcon, d, color);
		}
	}
}

void UnitCommandsPanel::UpdateCommands(GameViewContext& context)
{
	for (UnitCommand& cmd : unitCommands) {
		cmd.ability = nullptr;
		cmd.commandIcon = nullptr;
		cmd.abilityProduce = nullptr;
		cmd.enabled = false;
		cmd.active = false;
		cmd.pressed = false;


	}

	if (!context.HasSelectionControl())
		return;

	if (context.IsTargetSelectionMode) {
		unitCommands[8].enabled = true;
		unitCommands[8].pressed = unitCommands[8].active = InputManager::Gamepad.IsButtonDown(GamepadButton::B);
		unitCommands[8].commandIcon = &GameDatabase::instance->GetIcon(236);
		return;
	}

	if (context.selection.size() > 0)
	{
		EntityId entityId = context.selection[0];

		EntityManager& em = context.GetEntityManager();

		const UnitComponent& unit = em.UnitArchetype.UnitComponents.GetComponent(entityId);

		UnitAIState state = em.UnitArchetype.AIStateComponents.GetComponent(entityId);

		unitAbilities.clear();
		if (unit.def->GetAttacks().Size()  > 0)
		{
			unitAbilities.push_back(GameDatabase::instance->AttackAbility);
		}

		if (unit.def->Movement.MaxVelocity > 0)
		{
			unitAbilities.push_back(GameDatabase::instance->MoveAbility);
			unitAbilities.push_back(GameDatabase::instance->HoldPositionAbility);
			unitAbilities.push_back(GameDatabase::instance->PatrolAbility);
	
		}

		/*if (unit.def->ProductionUnit != nullptr) {
			if (!em.UnitArchetype.DataComponents.GetComponent(entityId).IsQueueFull())
			{
				unitCommands[0].ability = &AbilityDatabase::BuildUnit;
				unitCommands[0].enabled = true;
				unitCommands[0].abilityProduce = unit.def->ProductionUnit;
				unitCommands[0].commandIcon = &unit.def->ProductionUnit->Icon;
			}
		}*/

		if (unitAbilities.size() > 0)
			unitAbilities.push_back(GameDatabase::instance->StopAbility);

		for (const AbilityDef* ability : unitAbilities)
		{
			if (!ability)
				continue;

			unitCommands[ability->Art.GetButtonIndex()].ability = ability;
			unitCommands[ability->Art.GetButtonIndex()].enabled = true;
		}

		for (auto& cmd : unitCommands) {
			if (!cmd.IsUsable())
				continue;

			cmd.active = cmd.ability->Data.IsState(state);
		}
	}
}

void UnitCommandsPanel::OnCommandPressed(GameViewContext& context, const UnitCommand& cmd) {

	context.GetEntityManager().GetSoundSystem().PlayUISound(*Game::ButtonAudio);

	if (cmd.ability != nullptr) {

		if (cmd.abilityProduce != nullptr) {
			context.currentAbility = cmd.ability;
			context.ActivateCurrentAbility(*cmd.abilityProduce);
		}

		if (cmd.ability->Data.EntitySelectedAction == UnitAIState::Nothing)
			return;

		context.SelectAbilityTarget(*cmd.ability);

	}
	else {
		context.CancelTargetSelection();
	}
}