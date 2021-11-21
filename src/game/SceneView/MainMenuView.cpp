#include "MainMenuView.h"
#include "../Scenes/MainMenuScene.h"
#include "../Engine/AssetLoader.h"
#include "../Data/GameDatabase.h"

#include "../GUI/GUI.h"
#include "../Game.h"
#include "../Platform.h"
#include "../Engine/AssetLoader.h"
#include "../Engine/AudioManager.h"

#include "../Debug.h"


MainMenuView::MainMenuView(MainMenuScene* scene) :
	IMainMenuSceneView(scene)
{
	_single = AssetLoader::LoadVideoClip("glue\\mainmenu\\single");
	_singleon = AssetLoader::LoadVideoClip("glue\\mainmenu\\singleon");

	_multi = AssetLoader::LoadVideoClip("glue\\mainmenu\\multi");
	_multion = AssetLoader::LoadVideoClip("glue\\mainmenu\\multion");

	_editor = AssetLoader::LoadVideoClip("glue\\mainmenu\\editor");
	_editoron = AssetLoader::LoadVideoClip("glue\\mainmenu\\editoron");

	_exit = AssetLoader::LoadVideoClip("glue\\mainmenu\\exit");
	_exiton = AssetLoader::LoadVideoClip("glue\\mainmenu\\exiton");

}

MainMenuView::~MainMenuView()
{
	AssetLoader::UnloadVideoClip(_single);
	AssetLoader::UnloadVideoClip(_singleon);
	AssetLoader::UnloadVideoClip(_multi);
	AssetLoader::UnloadVideoClip(_multion);
	AssetLoader::UnloadVideoClip(_exit);
	AssetLoader::UnloadVideoClip(_exiton);

}


void MainMenuView::DrawGenric()
{
	GUI::UseScreen(ScreenId::Top);

	const Image& img = GameDatabase::instance->GetImage("glue\\palmm\\backgnd");

	GUIImage::DrawImage(img);

	GUI::BeginAbsoluteLayout({ 0,-20 }, _single->GetFrameSize()); {
		if (GUI::IsLayoutFocused())
		{
			GUI::BeginAbsoluteLayout({ 44,46 }, _singleon->GetFrameSize());
			GUIVideo::DrawVideo("singleon", *_singleon, true);
			GUI::EndLayout();
		}
		GUIVideo::DrawVideo("single", *_single, true);

		if (GUIButton::DrawMainMenuButtonTextOffset({ 56,60 }, "Single Player", true))
			_scene->StartGame();

	}
	GUI::EndLayout();

	GUI::BeginAbsoluteLayout({ 366,80 }, _editor->GetFrameSize()); {
		GUIVideo::DrawVideo("editor", *_editor, true);
		/*	if (GUI::IsLayoutFocused())
			{
				GUI::BeginAbsoluteLayout({ 386,96 }, _editoron->GetFrameSize());
				GUIVideo::DrawVideo("editoron", *_editoron, true);
				GUI::EndLayout();
			}*/
		GUIButton::DrawMainMenuButtonTextOffset({ 20,-58 }, "Campaign Editor", false);

	}
	GUI::EndLayout();

	GUI::BeginAbsoluteLayout({ 78,190 }, _multi->GetFrameSize()); {
		GUIVideo::DrawVideo("multi", *_multi, true);
		if (GUI::IsLayoutFocused())
		{
			GUI::BeginAbsoluteLayout({ 98,202 }, _multion->GetFrameSize());
			GUIVideo::DrawVideo("multion", *_multion, true);
			GUI::EndLayout();
		}
		if (GUIButton::DrawMainMenuButtonTextOffset({ -76,-78 }, "Multiplayer", true))
			_scene->ToMultiplayerConnection();
	}
	GUI::EndLayout();


	const Image& etail = GameDatabase::instance->GetImage("glue\\mainmenu\\etail");

	GUI::BeginAbsoluteLayout({ 225,348 }, Vector2Int(etail.GetSize()));
	GUIImage::DrawImage(etail);
	GUI::EndLayout();


	GUI::BeginAbsoluteLayout({ 416,340 }, Vector2Int( _exit->GetFrameSize()));
	{
		GUIVideo::DrawVideo("exit", *_exit, true);
		if (GUI::IsLayoutFocused())
		{
			GUI::BeginAbsoluteLayout({ 424,328 }, Vector2Int{ Vector2(_exiton->GetFrameSize()) });
			GUIVideo::DrawVideo("exiton", *_exiton, true);
			GUI::EndLayout();
		}

		if (GUIButton::DrawMainMenuButtonTextOffset({ 20,-58 }, "Exit", true))
			Game::Exit();

	}
	GUI::EndLayout();

	GUI::BeginRelativeLayout({}, { 200,50 }, GUIHAlign::Left, GUIVAlign::Bottom);
	if (GUIButton::DrawMainMenuButton({ 20,-32 }, { 120,20 }, GUIHAlign::Center, GUIVAlign::Top, "View Intro", true))
	{
		Game::ShowIntro();
	}
	GUIButton::DrawMainMenuButton({ 20,-32 }, { 160,20 }, GUIHAlign::Center, GUIVAlign::Bottom, "Show Credits", false);
	GUI::EndLayout();

	GUILabel::DrawMenuText("Tech Demo 1", { -2,-2 }, GUIHAlign::Right, GUIVAlign::Bottom);
}

void MainMenuView::DrawN3DS()
{
	const Image& img = GameDatabase::instance->GetImage("glue\\palmm\\backgnd");

	GUI::UseScreen(ScreenId::Top);


	GUI::BeginRelativeLayout({ 0,0 }, Vector2Int(img.GetSize()));
	GUIImage::DrawImage(img);
	GUI::EndLayout();

	GUI::BeginAbsoluteLayout({ 25,20 }, _single->GetFrameSize()); {
		//if (GUI::IsLayoutFocused())
		{
			GUI::BeginAbsoluteLayout({ 69,86 }, _singleon->GetFrameSize());
			GUIVideo::DrawVideo("singleon", *_singleon, true);
			GUI::EndLayout();
		}
		GUIVideo::DrawVideo("single", *_single, true);

		GUIButton::SetNextButtonSelected();
		GUIButton::DrawMainMenuButtonTextOffset({ 76,60 }, "Single Player", true);

	

	}
	GUI::EndLayout();


	GUI::UseScreen(ScreenId::Bottom);

	GUI::BeginAbsoluteLayout({ -320,-60 }, Vector2Int(img.GetSize()) );
	GUIImage::DrawImage(img);
	GUI::EndLayout();

	GUI::BeginAbsoluteLayout({ 66,60 }, Vector2Int (_exit->GetFrameSize()) );
	{
		GUIVideo::DrawVideo("exit", *_exit, true);
	/*	if (GUI::IsLayoutFocused())
		{
			GUI::BeginAbsoluteLayout({ 74,48 }, Vector2Int{ Vector2(_exiton->GetFrameSize()) });
			GUIVideo::DrawVideo("exiton", *_exiton, true);
			GUI::EndLayout();
		}*/

		if (GUIButton::DrawMainMenuButtonTextOffset({ 10,-58 }, "Exit", true))
		{
			Game::Exit();
		}
	}
	GUI::EndLayout();

	GUILabel::DrawMenuText("Tech Demo 1", { -2,-2 }, GUIHAlign::Right, GUIVAlign::Bottom);


	if (Game::GetInput().Common.Comfirm.IsActivated()) {
		if (_menuIndex == 0)
			_scene->StartGame();
		else

			Game::Exit();
	}
		
}

void MainMenuView::Draw()
{
	GUI::SetVideoPlaybackSpeed(0.8);

	if (Game::GetPlatformInfo().Type == PlatformType::Nintendo3DS)
		DrawN3DS();
	else
		DrawGenric();

	GUI::SetVideoPlaybackSpeed(1);
}
