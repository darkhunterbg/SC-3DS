#include "GameView.h"

#include "../Entity/EntityManager.h"
#include "../Entity/EntityUtil.h"
#include "../Game.h"
#include "../Platform.h"

GameView::GameView()
{
}

void GameView::Init()
{
	Vector2Int16 size = EntityUtil::GetManager().MapSystem.GetSize();
	_camera.Position = { 0,0 };
	_camera.Limits = { {0,0,}, size };

	SetPlayer(_context.GetPlayer());
}

void GameView::SetPlayer(PlayerId player)
{
	_context.SetPlayer(player);
	EntityUtil::GetManager().MapSystem.ActivePlayer = player;
}

void GameView::DrawMainScreen()
{
	_camera.Size = Game::GetPlatformInfo().Screens[0];
}

void GameView::DrawSecondaryScreen()
{
}
