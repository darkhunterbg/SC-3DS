#include "UnitCommandsPanel.h"
#include "../Platform.h"
#include "../Color.h"
#include "../Game.h"

#include "../Entity/EntityManager.h"

 UnitCommandsPanel::UnitCommandsPanel() {
	 for (UnitCommand& cmd : unitCommands) {
		 cmd.sprite = nullptr;
		 cmd.enabled = false;
		 cmd.active = false;
		 cmd.pressed = false;
	 }
}

void UnitCommandsPanel::Draw(GameHUDContext& context)
{
	DrawCommands(context);
}

void UnitCommandsPanel::UpdateInput(GameHUDContext& context)
{
	UpdateCommands(context);

	if (Game::Pointer.IsPressed())
	{
		Rectangle dst = PanelDst;

		const Sprite& f = context.GetCommandIconsAtlas().GetFrame(0).sprite;

		dst.size = Vector2Int(f.rect.size);

		for (int i = 0; i < 9; ++i) {
			int x = i % 3;
			int y = i / 3;
			const UnitCommand& cmd = unitCommands[i];

			if (cmd.sprite == nullptr || !cmd.enabled)
				continue;

			Vector2Int offset = { x * 46, y * 40 };
			Rectangle d = dst;
			d.position += offset;

			if (d.Contains(Game::Pointer.Position())) {
				pressedCommand = i;
				break;
			}

		}
	}

	if (pressedCommand != -1) {
		unitCommands[pressedCommand].pressed = true;
		unitCommands[pressedCommand].active = true;
	}
	
	if (pressedCommand != -1 && Game::Pointer.IsReleased()) {
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

			if (cmd.sprite == nullptr)
				continue;

			Vector2Int offset = { x * 46, y * 40 };


			const Sprite& f = cmd.pressed ? pressed : normal;
			dst.size =  Vector2Int(f.rect.size);

			Rectangle d = dst;
			d.position += offset;
			Platform::Draw(f, d);

			d.size = Vector2Int(cmd.sprite->rect.size);
			d.position += {3, 3};

			
			if (cmd.pressed) {
				// TODO: sprite frames are needed here
				d.position += {1, 1};
			}

			Color color = Colors::IconGray;

			if (cmd.enabled)
				color = Colors::IconYellow;

			if (cmd.active)
				color = Colors::White;



			Platform::Draw(*cmd.sprite, d, color);
		}
	}
}

void UnitCommandsPanel::UpdateCommands(GameHUDContext& context)
{
	for (UnitCommand& cmd : unitCommands) {
		cmd.sprite = nullptr;
		cmd.enabled = false;
		cmd.active = false;
		cmd.pressed = false;
	}

	const auto atlas = Game::AssetLoader.LoadAtlas("unit_cmdbtns_cmdicons.t3x");

	if (context.selectedEntities.size() > 0)
	{
		EntityId entityId = context.selectedEntities[0];

		EntityManager& em = context.GetEntityManager();

		const UnitComponent& unit = em.UnitArchetype.UnitComponents.GetComponent(entityId);

		UnitAIState state = em.UnitArchetype.AIStateComponents.GetComponent(entityId);

		unitCommands[0].sprite = &atlas->GetSprite(228);
		unitCommands[0].enabled = true;
		unitCommands[0].active = state == UnitAIState::GoToPosition || state == UnitAIState::GoToAttack;

		unitCommands[1].sprite = &atlas->GetSprite(229);
		unitCommands[1].enabled = true;
		unitCommands[1].active = state == UnitAIState::Idle;

		if (unit.def->Weapon) {
			unitCommands[2].sprite = &atlas->GetSprite(230);
			unitCommands[2].enabled = true;
			unitCommands[2].active = state == UnitAIState::AttackTarget;
		}

		unitCommands[3].sprite = &atlas->GetSprite(254);
		unitCommands[3].enabled = true;

		//236

		unitCommands[4].sprite = &atlas->GetSprite(255);
		unitCommands[4].enabled = true;
	}
}
