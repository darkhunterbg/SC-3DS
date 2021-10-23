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


	GUI::SetVideoPlaybackSpeed(0.75);
}

MainMenuSceneView::~MainMenuSceneView()
{
	GUI::SetVideoPlaybackSpeed(1);
}

void MainMenuSceneView::Draw()
{
	GUI::UseScreen(ScreenId::Top);

	const Image& img = GameDatabase::instance->GetImage("glue\\palmm\\backgnd");

	GUIImage::DrawImage(img);

	Color buttonColor = Colors::UIMenuGreen;

	GUI::BeginAbsoluteLayout({ 0,-20 }, _single->GetFrameSize());
	if (GUI::IsLayoutHover())
	{
		GUI::BeginAbsoluteLayout({ 44,46 }, _singleon->GetFrameSize());
		GUIVideo::DrawVideo("singleon", *_singleon, true);
		GUI::EndLayout();
		buttonColor = Colors::UIMenuGreenLit;
	}
	GUIVideo::DrawVideo("single", *_single, true);

	GUILabel::DrawText(*_mmFont, "Single Player", { -26,16 }, GUIHAlign::Right, GUIVAlign::Bottom, buttonColor);
	GUI::EndLayout();

	buttonColor = Colors::UIMenuGreen;

	GUI::BeginAbsoluteLayout({ 78,190 }, _multi->GetFrameSize());
	GUIVideo::DrawVideo("multi", *_multi, true);
	if (GUI::IsLayoutHover())
	{
		GUI::BeginAbsoluteLayout({ 98,202 }, _multion->GetFrameSize());
		GUIVideo::DrawVideo("multion", *_multion, true);
		GUI::EndLayout();
		buttonColor = Colors::UIMenuGreenLit;
	}
	
	GUILabel::DrawText(*_mmFont, "Multiplayer", { 0,0 }, GUIHAlign::Left, GUIVAlign::Top, buttonColor);
	GUI::EndLayout();

	
	const Image& etail = GameDatabase::instance->GetImage("glue\\mainmenu\\etail");

	GUI::BeginAbsoluteLayout({ 224,348 }, Vector2Int(etail.GetSize()));
	GUIImage::DrawImage(etail);
	GUI::EndLayout();


	buttonColor = Colors::UIMenuGreen;

	GUI::BeginAbsoluteLayout({ 416,340 }, Vector2Int{ Vector2(_multi->GetFrameSize()) * 0.66f });
	GUIVideo::DrawVideo("exit", *_exit, true);
	if (GUI::IsLayoutHover())
	{
		GUI::BeginAbsoluteLayout({ 424,328 },Vector2Int{ Vector2(_exiton->GetFrameSize())  });
		GUIVideo::DrawVideo("exiton", *_exiton, true);
		GUI::EndLayout();
		buttonColor = Colors::UIMenuGreenLit;
	}
		
	GUILabel::DrawText(*_mmFont, "Exit", { -50,-8 }, GUIHAlign::Right, GUIVAlign::Top, buttonColor);
	GUI::EndLayout();

	buttonColor = Colors::UIMenuGreen;

	GUI::BeginRelativeLayout({}, { 200,50 }, GUIHAlign::Left, GUIVAlign::Bottom);
	GUILabel::DrawText(*_mmFont, "View Intro", { 0,-32 }, GUIHAlign::Center, GUIVAlign::Top, buttonColor);
	GUILabel::DrawText(*_mmFont, "Show Credits", { 0,-32 }, GUIHAlign::Center, GUIVAlign::Bottom, buttonColor);
	GUI::EndLayout();

	if (Game::GetPlatformInfo().Type == PlatformType::Nintendo3DS)
	{
		GUI::UseScreen(ScreenId::Bottom);
		GUIImage::DrawColor(Colors::Black);
	}
}
