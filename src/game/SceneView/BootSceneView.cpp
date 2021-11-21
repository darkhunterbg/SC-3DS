#include "BootSceneView.h"
#include "../Scenes/BootScene.h"
#include "../Assets.h"
#include "../Engine/AssetLoader.h"
#include "../Game.h"
#include "../Platform.h"
#include "../GUI/GUI.h"

BootSceneView::BootSceneView(BootScene* scene) : _scene(scene)
{
	_title = AssetLoader::LoadTexture("atlases\\title_0");
}

BootSceneView::~BootSceneView()
{
	AssetLoader::UnloadTexture(_title);
}

void BootSceneView::Draw()
{
	if (Game::GetPlatformInfo().Type == PlatformType::Nintendo3DS)
	{
		GUI::UseScreen(ScreenId::Top);

		GUIImage::DrawTextureAbsolute(*_title, { {0,0},{400,300} });

		GUI::UseScreen(ScreenId::Bottom);

		GUIImage::DrawColor(Colors::Black);

		GUIImage::DrawTextureAbsolute(*_title, { { -440, -358} ,{640,480} });

		GUIImage::DrawColorAbsolute({ {0,0},{120,200} }, Colors::Black);

		GUIImage::DrawTextureAbsolute(*_title, { { -40,-240 }, { 400,300 } });
		GUIImage::DrawColorAbsolute({ {300,25},{20,100} }, Colors::Black);

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
			GUILabel::DrawMenuText( text, Vector2Int(-2, 20), GUIHAlign::Center, GUIVAlign::Center);
		}
	}
	else
	{
		GUI::UseScreen(ScreenId::Top);

		GUIImage::DrawTexture(*_title);

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
			GUILabel::DrawText(*Game::SystemFont16, text, Vector2Int(0, 160), GUIHAlign::Center, GUIVAlign::Center, Colors::SCWhite);
		}
	}

	if (_scene->IsLoadCompleted() && Game::GetInput().Common.Comfirm.IsActivated())
	{
		_scene->StartGame();
	}

	_frameCounter++;
}
