#include "MultiplayerConnectionView.h"

#include "../Scenes/MainMenuScene.h"

#include "../Data/GameDatabase.h"
#include "../GUI/GUI.h"
#include "../Game.h"

#include "../MathLib.h"

#include "../Engine/AudioManager.h"


static constexpr const float SlideInTime = 0.7f;
static constexpr const float SlideOutTime = 0.4f;


MultiplayerConnectionView::MultiplayerConnectionView(MainMenuScene* scene) : IMainMenuSceneView(scene)
{

}

MultiplayerConnectionView::~MultiplayerConnectionView()
{
}

void MultiplayerConnectionView::OnShow()
{
	Game::PlayUISound(GameDatabase::instance->GetSoundSet("Misc\\SlideIn"));
	_time = 0;
}

bool MultiplayerConnectionView::DoneHiding()
{
	if (!_hideStart)
	{
		_hideStart = true;
		Game::PlayUISound(GameDatabase::instance->GetSoundSet("Misc\\SlideOut"));

		_time = 0;

	}

	return IsSlideDone();
}


bool MultiplayerConnectionView::IsSlideDone() const
{
	float lerpTime = _hideStart ? SlideOutTime : SlideInTime;
	return _time > lerpTime;
}


void MultiplayerConnectionView::Draw()
{
	float lerpTime = _hideStart ? SlideOutTime : SlideInTime;

	_time += Game::DeltaTime;

	float lerp = std::fminf(1.0f, _time / lerpTime);
	lerp = 1.0f - lerp;
	lerp *= lerp;
	lerp *= lerp;

	if (!_hideStart)
		lerp = 1.0f - lerp;

	const Font& font = *Game::SystemFont16;

	GUI::UseScreen(ScreenId::Top);

	const Image& bg = GameDatabase::instance->GetImage("glue\\palnl\\backgnd");

	GUIImage::DrawImage(bg);


	Vector2Int okCancelPos = { (int)LerpF(210,0, lerp) ,0 };
	GUI::BeginRelativeLayout(okCancelPos, { 180,160 }, GUIHAlign::Right, GUIVAlign::Bottom);
	{
		//GUI::DrawLayoutDebug();

		const Image& ok = GameDatabase::instance->GetImage("glue\\palnl\\pok");
		GUI::BeginRelativeLayout({ 0,0 }, Vector2Int(ok.GetSize()), GUIHAlign::Right, GUIVAlign::Top); {
			GUIImage::DrawImage(ok);

			if (GUIButton::DrawMainMenuButton({ -20,22 }, { 130,22 }, GUIHAlign::Center, GUIVAlign::Center, "Ok", false))
				_scene->ToMainMenu();

		}
		GUI::EndLayout();

		const Image& cancel = GameDatabase::instance->GetImage("glue\\palnl\\pcancel");
		GUI::BeginRelativeLayout({ 0,22 }, Vector2Int(cancel.GetSize()), GUIHAlign::Right, GUIVAlign::Top);
		{
			GUIImage::DrawImage(cancel);

			if (GUIButton::DrawMainMenuButton({ 8,28 }, { 130,20 }, GUIHAlign::Left, GUIVAlign::Center, "Cancel", true))
				_scene->ToMainMenu();
		}
		GUI::EndLayout();
	}
	GUI::EndLayout();
}

