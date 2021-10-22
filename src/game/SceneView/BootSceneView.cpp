#include "BootSceneView.h"
#include "../Scenes/BootScene.h"
#include "../Assets.h"
#include "../Engine/AssetLoader.h"
#include "../Engine/InputManager.h"
#include "../Game.h"
#include "../Platform.h"
#include "../GUI/GUI.h"

static const Texture* _title;

BootSceneView::BootSceneView(BootScene* scene) : _scene(scene)
{
	_title = AssetLoader::LoadTexture("atlases\\title_0");
}

void BootSceneView::Draw()
{
	if (Game::GetPlatformInfo().Type == PlatformType::Nintendo3DS)
	{
		GUI::UseScreen(ScreenId::Top);

		GUIImage::DrawTexture(*_title, { {0,0},{400,300} });

		GUI::UseScreen(ScreenId::Bottom);

		GUIImage::DrawColor(Colors::Black);

		GUIImage::DrawTexture(*_title, { { -440, -358} ,{640,480} });

		GUIImage::DrawColor({ {0,0},{120,200} }, Colors::Black);

		GUIImage::DrawTexture(*_title, { { -40,-240 }, { 400,300 } });
		GUIImage::DrawColor({ {300,25},{20,100} }, Colors::Black);

		const char* text = nullptr;

		if (_scene->IsLoadCompleted())
		{
			if ((_frameCounter / 30) % 3)
				text = "Press A to continue";
		}
		else
		{
			text = "Loading";
		}

		if (text != nullptr)
		{
			GUILabel::DrawText(*Game::SystemFont12, text, Vector2Int(0, 20), GUIHorizontalAlignment::Center, GUIVerticalAlignment::Center, Colors::SCWhite);
		}
	}
	else
	{
		GUI::UseScreen(ScreenId::Top);

		GUIImage::DrawTexture(*_title);

		Vector2Int pos = Vector2Int(_title->GetSize());
		pos.x /= 2;
		pos.y -= 90;
		pos.x -= 22;

		const char* text = nullptr;

		if (_scene->IsLoadCompleted())
		{
			if ((_frameCounter / 30) % 3)
				text = "Press A to continue";

		}
		else
		{
			text = "Loading";
		}

		if (text != nullptr)
		{
			GUILabel::DrawText(*Game::SystemFont16, text, Vector2Int(0, 160), GUIHorizontalAlignment::Center, GUIVerticalAlignment::Center, Colors::SCWhite);
		}
	}

	if (_scene->IsLoadCompleted() && (InputManager::Gamepad.IsButtonDown(GamepadButton::A)))
	{
		_scene->StartGame();
	}

	_frameCounter++;
}