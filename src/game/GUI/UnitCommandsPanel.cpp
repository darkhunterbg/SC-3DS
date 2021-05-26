#include "UnitCommandsPanel.h"
#include "../Platform.h"
#include "../Color.h"
#include "../Game.h"

#include "../Entity/EntityManager.h"

void UnitCommandsPanel::Draw(const std::vector<EntityId>& selection, EntityManager& em)
{
	for (UnitCommand& cmd : unitCommands) {
		cmd.sprite = nullptr;
		cmd.enabled = false;
		cmd.active = false;
	}

	const auto atlas = Game::AssetLoader.LoadAtlas("unit_cmdbtns_cmdicons.t3x");

	if (selection.size() > 0)
	{
		EntityId entityId = selection[0];

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

		unitCommands[4].sprite = &atlas->GetSprite(255);
		unitCommands[4].enabled = true;
	}

	DrawCommands();
}


void UnitCommandsPanel::DrawCommands() {

	Rectangle dst = PanelDst;

	const Sprite& f = Race->CommandIconsAtlas->GetFrame(0).sprite;

	dst.size = Vector2Int(f.rect.size);

	for (int y = 0; y < 3; y++) {
		for (int x = 0; x < 3; ++x) {

			const UnitCommand& cmd = unitCommands[x + y * 3];

			if (cmd.sprite == nullptr)
				continue;

			Vector2Int offset = { x * 45, y * 40 };
			Rectangle d = dst;
			d.position += offset;

			Platform::Draw(f, d);
			d.size = Vector2Int(cmd.sprite->rect.size);
			d.position += Vector2Int(f.rect.size - cmd.sprite->rect.size) / 2;

			Color color = Colors::IconGray;

			if (cmd.enabled)
				color = Colors::IconYellow;

			if (cmd.active)
				color = Colors::White;

			Platform::Draw(*cmd.sprite, d, color);
		}
	}
}