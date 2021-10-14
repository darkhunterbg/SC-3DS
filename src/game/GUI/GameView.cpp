#include "GameView.h"

#include "../Entity/EntityManager.h"
#include "../Entity/EntityUtil.h"
#include "../Game.h"
#include "../Platform.h"
#include "../Engine/InputManager.h"

GameView::GameView()
{
}

void GameView::Init()
{
	Vector2Int16 size = EntityUtil::GetManager().MapSystem.GetSize();
	_camera.Position = { 0,0 };
	_camera.Limits = { {0,0,}, size };
	_cursor.Limits = _camera.Limits;

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


	if (Game::GetPlatformInfo().PointerIsCursor)
	{
		_cursor.Position = Vector2Int16(InputManager::Pointer.Position());
	}
	else
	{
		// TODO: Instead of polling directly, use commands
		
		Vector2 move = InputManager::Gamepad.CPad();
		_cursor.Position += Vector2Int16(move * 10);
	}

	_cursor.Draw();

}

void GameView::DrawSecondaryScreen()
{
}
