#include "MainMenuSceneView.h"
#include "../Scenes/MainMenuScene.h"
#include "../Engine/AssetLoader.h"
#include "../Data/GameDatabase.h"

#include "../GUI/GUI.h"
#include "../Game.h"
#include "../Platform.h"
#include "../Engine/AssetLoader.h"

MainMenuSceneView::MainMenuSceneView(MainMenuScene* scene):
	_scene (scene)
{
	_single = AssetLoader::LoadVideoClip("glue\\mainmenu\\single");
	_singleon = AssetLoader::LoadVideoClip("glue\\mainmenu\\singleon");

	_multi = AssetLoader::LoadVideoClip("glue\\mainmenu\\multi");
	_multion = AssetLoader::LoadVideoClip("glue\\mainmenu\\multion");

	_exit = AssetLoader::LoadVideoClip("glue\\mainmenu\\exit");
	_exiton = AssetLoader::LoadVideoClip("glue\\mainmenu\\exiton");

	_mmFont = AssetLoader::LoadFont("mm-font", 16);
}

MainMenuSceneView::~MainMenuSceneView()
{
}

void MainMenuSceneView::Draw()
{
	GUI::UseScreen(ScreenId::Top);

	const Image& img = GameDatabase::instance->GetImage("glue\\palmm\\backgnd");

	GUIImage::DrawImage(img);

	GUI::BeginAbsoluteLayout({ 0,-20 }, _single->GetFrameSize());
	GUIVideo::DrawVideo("single", * _single, true);
	GUILabel::DrawText(*_mmFont, "Single Player", { -26,16 }, GUIHAlign::Right, GUIVAlign::Bottom, Colors::UIMenuGreen);
	GUI::EndLayout();

	GUI::BeginAbsoluteLayout({ 78,190 }, _multi->GetFrameSize());
	GUIVideo::DrawVideo("multi", *_multi, true);
	GUILabel::DrawText(*_mmFont, "Multiplayer", { 0,0 }, GUIHAlign::Left, GUIVAlign::Top, Colors::UIMenuGreen);
	GUI::EndLayout();

	const Image& etail = GameDatabase::instance->GetImage("glue\\mainmenu\\etail");

	GUI::BeginAbsoluteLayout({ 224,348 }, Vector2Int(etail.GetSize()));
	GUIImage::DrawImage(etail);
	GUI::EndLayout();


	GUI::BeginAbsoluteLayout({ 416,340 }, Vector2Int{ Vector2(_multi->GetFrameSize()) * 0.66f });
	GUIVideo::DrawVideo("exit", *_exit, true);
	GUILabel::DrawText(*_mmFont, "Exit", { -50,-8 }, GUIHAlign::Right, GUIVAlign::Top, Colors::UIMenuGreen);
	GUI::EndLayout();


	GUI::BeginRelativeLayout({}, { 200,50 }, GUIHAlign::Left, GUIVAlign::Bottom);
	GUILabel::DrawText(*_mmFont, "View Intro", { 0,-32 }, GUIHAlign::Center, GUIVAlign::Top, Colors::UIMenuGreen);
	GUILabel::DrawText(*_mmFont, "Show Credits", { 0,-32 }, GUIHAlign::Center, GUIVAlign::Bottom, Colors::UIMenuGreen);
	GUI::EndLayout();

	if (Game::GetPlatformInfo().Type == PlatformType::Nintendo3DS)
	{
		GUI::UseScreen(ScreenId::Bottom);
		GUIImage::DrawColor(Colors::Black);
	}
}
