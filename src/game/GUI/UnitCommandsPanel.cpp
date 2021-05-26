#include "UnitCommandsPanel.h"
#include "../Platform.h"
#include "../Color.h"
#include "../Game.h"

#include "../Data/AbilityDatabase.h"
#include "../Entity/EntityManager.h"
#include "../Data/Generated.h"
#include "../Entity/Common.h"

#include "../Entity/EntityUtil.h"

UnitCommandsPanel::UnitCommandsPanel() {

}

void UnitCommandsPanel::Draw(GameHUDContext& context)
{
	DrawCommands(context);
}

void UnitCommandsPanel::UpdateInput(GameHUDContext& context)
{
	UpdateCommands(context);


	Rectangle dst = PanelDst;

	const Sprite& f = context.GetCommandIconsAtlas().GetFrame(0).sprite;

	dst.size = Vector2Int(f.rect.size);

	if (Game::Pointer.IsDown()) {

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

			if (d.Contains(Game::Pointer.Position())) {
				hover = i;
				break;
			}

		}

		if (Game::Pointer.IsPressed() && hover != -1)
		{
			pressedCommand = hover;
		}
	}

	
	if (pressedCommand != -1) {
		unitCommands[pressedCommand].pressed = true;
		unitCommands[pressedCommand].active = true;
	}

	if (pressedCommand != -1 && Game::Pointer.IsReleased() ) {
		if (pressedCommand == hover) {
			OnCommandPressed(context, unitCommands[pressedCommand]);
		}

		pressedCommand = -1;
	}
}

void UnitCommandsPanel::DrawCommands(GameHUDContext& context) {

	Rectangle dst = PanelDst;

	const Sprite& normal = context.GetCommandIconsAtlas().GetFrame(0).sprite;
	const Sprite& pressed = context.GetCommandIconsAtlas().GetFrame(1).sprite;

	for (int y = 0; y < 3; y++) {
		for (int x = 0; x < 3; ++x) {

			const UnitCommand& cmd = unitCommands[x + y * 3];

			if (!cmd.IsVisible())
				continue;

			Vector2Int offset = { x * 46, y * 40 };


			const Sprite& f = cmd.pressed ? pressed : normal;
			dst.size = Vector2Int(f.rect.size);

			Rectangle d = dst;
			d.position += offset;
			Platform::Draw(f, d);

			const SpriteFrame& commandIcon = cmd.commandIcon !=nullptr ?
				*cmd.commandIcon : cmd.ability->Sprite;

			d.size = Vector2Int(commandIcon.sprite.rect.size);
			d.position = dst.position + offset +
				(Vector2Int{ 36, 35 } - Vector2Int(commandIcon.offset + commandIcon.sprite.rect.size)) / 2;


			if (cmd.pressed) {
				// TODO: sprite frames are needed here
				d.position += {1, 1};
			}

			Color color = Colors::IconGray;

			if (cmd.enabled)
				color = Colors::IconYellow;

			if (cmd.active)
				color = Colors::White;



			Platform::Draw(commandIcon.sprite, d, color);
		}
	}
}

void UnitCommandsPanel::UpdateCommands(GameHUDContext& context)
{
	for (UnitCommand& cmd : unitCommands) {
		cmd.ability = nullptr;
		cmd.commandIcon = nullptr;
		cmd.enabled = false;
		cmd.active = false;
		cmd.pressed = false;
	
	}

	if (context.IsTargetSelectionMode) {
		unitCommands[8].enabled = true;
		unitCommands[8].commandIcon = &SpriteDatabase::Load_unit_cmdbtns_cmdicons()->GetFrame(236);
		return;
	}
	
	if (context.selectedEntities.size() > 0)
	{
		EntityId entityId = context.selectedEntities[0];

		EntityManager& em = context.GetEntityManager();

		const UnitComponent& unit = em.UnitArchetype.UnitComponents.GetComponent(entityId);

		UnitAIState state = em.UnitArchetype.AIStateComponents.GetComponent(entityId);

		unitCommands[0].ability = &AbilityDatabase::Move;
		unitCommands[0].enabled = true;

		unitCommands[1].ability = &AbilityDatabase::Stop;
		unitCommands[1].enabled = true;

		if (unit.def->Weapon) {
			unitCommands[2].ability = &AbilityDatabase::Attack;
			unitCommands[2].enabled = true;
		}

		unitCommands[3].ability = &AbilityDatabase::Patrol;
		unitCommands[3].enabled = true;

		unitCommands[4].ability = &AbilityDatabase::HoldPosition;
		unitCommands[4].enabled = true;

		for (auto& cmd : unitCommands) {
			if (!cmd.IsUsable())
				continue;

			cmd.active = cmd.ability->TargetingData.IsState(state);
		}
	}
}

void UnitCommandsPanel::OnCommandPressed(GameHUDContext& context, const UnitCommand& cmd) {

	if (cmd.ability != nullptr) {

		if (cmd.ability->TargetingData.EntitySelectedAction == UnitAIState::Nothing)
			return;

		context.SelectAbilityTarget(*cmd.ability);

	}
	else {
		context.CancelTargetSelection();
	}
}