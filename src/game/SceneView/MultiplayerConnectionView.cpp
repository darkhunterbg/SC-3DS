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
	_textInput[0] = '\0';
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


	const Image& list = GameDatabase::instance->GetImage("glue\\selconn\\plistsml");
	Vector2Int listPos = { (int)LerpF(-list.GetSize().x ,0, lerp) ,0 };

	if (_hostClientModal || _connectIPModal)
		GUI::SetInputEnabled(false);

	GUI::BeginRelativeLayout(listPos, Vector2Int(list.GetSize()), GUIHAlign::Left, GUIVAlign::Top);
	{
		GUIImage::DrawImage(list);

		GUI::SetNextFont(Game::MenuFont16);
		GUILabel::DrawMenuText("Select Connection", { 50,56 }, GUIHAlign::Left, GUIVAlign::Top);

		Vector2Int offset = { 52,90 };
		GUI::SetNextFont(Game::MenuFont16);
		GUIButton::DrawMainMenuButtonFromText(offset, GUIHAlign::Left, GUIVAlign::Top, "Battle.net", false);

		offset.y += 20;
		GUIButton::DrawMainMenuButtonFromText(offset, GUIHAlign::Left, GUIVAlign::Top, "Local Area Network (IPX)", false);

		offset.y += 20;
		GUIButton::DrawMainMenuButtonFromText(offset, GUIHAlign::Left, GUIVAlign::Top, "Modem", false);

		offset.y += 20;
		GUIButton::DrawMainMenuButtonFromText(offset, GUIHAlign::Left, GUIVAlign::Top, "Direct Cable Connection", false);

		offset.y += 20;
		GUIButton::DrawMainMenuButtonFromText(offset, GUIHAlign::Left, GUIVAlign::Top, "Local Area Network (UDP)", false);

		GUIButton::SetNextButtonSelected();
		offset.y += 20;
		GUIButton::DrawMainMenuButtonFromText(offset, GUIHAlign::Left, GUIVAlign::Top, "Direct IP Connection", true);
	}
	GUI::EndLayout();


	const Image& info = GameDatabase::instance->GetImage("glue\\selconn\\pinfo");
	Vector2Int infoPos = { (int)LerpF(info.GetSize().x ,0, lerp) ,30 };

	GUI::BeginRelativeLayout(infoPos, Vector2Int(info.GetSize()), GUIHAlign::Right, GUIVAlign::Top);
	{
		GUIImage::DrawImage(info);

		GUI::BeginRelativeLayout({ 10,54 }, { 240,256 }, GUIHAlign::Left, GUIVAlign::Top);

		GUI::SetNextFont(Game::MenuFont16);

		GUILabel::DrawMenuText("Direct IP\nConnection", { 0, 0 }, GUIHAlign::Left, GUIVAlign::Top);
		GUILabel::DrawMenuText("Supports up to 8 playes", { 0,56 }, GUIHAlign::Center, GUIVAlign::Top);

		GUILabel::DrawMenuText("Requirements: All computers must be\nconnected to a TCP/IP compatible\nnetwork. Host must have public IP\naddress.",
			{ 0,90 }, GUIHAlign::Left, GUIVAlign::Top);


		GUI::EndLayout();
	}
	GUI::EndLayout();


	Vector2Int okCancelPos = { (int)LerpF(210,0, lerp) ,14 };
	GUI::BeginRelativeLayout(okCancelPos, { 180,160 }, GUIHAlign::Right, GUIVAlign::Bottom);
	{
		const Image& ok = GameDatabase::instance->GetImage("glue\\palmm\\pok");
		GUI::BeginRelativeLayout({ 0,0 }, Vector2Int(ok.GetSize()), GUIHAlign::Right, GUIVAlign::Top); {
			GUIImage::DrawImage(ok);

			if (GUIButton::DrawMainMenuButton({ -20,22 }, { 130,22 }, GUIHAlign::Center, GUIVAlign::Center, "Ok", true))
			{
				_hostClientModal = true;
			}
			//_scene->StartGame();
		}
		GUI::EndLayout();

		const Image& cancel = GameDatabase::instance->GetImage("glue\\palmm\\pcancel");
		GUI::BeginRelativeLayout({ 0,22 }, Vector2Int(cancel.GetSize()), GUIHAlign::Right, GUIVAlign::Top);
		{
			GUIImage::DrawImage(cancel);

			if (GUIButton::DrawMainMenuButton({ 8,28 }, { 130,20 }, GUIHAlign::Left, GUIVAlign::Center, "Cancel", true))
				_scene->ToMainMenu();
		}
		GUI::EndLayout();
	}
	GUI::EndLayout();

	GUI::SetInputEnabled(true);

	if (_connectIPModal)
	{
		const Image& popup = GameDatabase::instance->GetImage("glue\\palmm\\pepopup");
		GUI::BeginRelativeLayout({ 0,0 }, Vector2Int(popup.GetSize())); {
			GUIImage::DrawImage(popup);

			GUI::SetNextFont(Game::MenuFont16);
			GUILabel::DrawMenuText("Enter IP address to connect", { 0,-60 }, GUIHAlign::Center, GUIVAlign::Center);

			GUI::BeginRelativeLayout({ 0,-12 }, { 320,24 });
			GUITextbox::DrawTextbox("ConnectIP", *Game::MenuFont16, { _textInput.data(), _textInput.size() }, Colors::UILightGray);
			GUI::EndLayout();

			if (GUIButton::DrawMainMenuButton({ 16,-12 }, { 160,30 }, GUIHAlign::Left, GUIVAlign::Bottom, "Cancel"))
			{
				_connectIPModal = false;
			}
			if (GUIButton::DrawMainMenuButton({ -16,-12 }, { 160,30 }, GUIHAlign::Right, GUIVAlign::Bottom, "Ok", _textInput[0] != 0))
			{
				_connectIPModal = false;
			}
		}
		GUI::EndLayout();
	}

	if (_hostClientModal)
	{
		const Image& popup = GameDatabase::instance->GetImage("glue\\palmm\\pdpopup");
		GUI::BeginRelativeLayout({ 0,0 }, Vector2Int(popup.GetSize())); {
			GUIImage::DrawImage(popup);

			GUI::SetNextFont(Game::MenuFont16);
			GUILabel::DrawMenuText("Are you hosting or\n   joining a game?", { 0,-20 }, GUIHAlign::Center, GUIVAlign::Center);

		

			if (GUIButton::DrawMainMenuButton({ 16,-12 }, { 160,30 }, GUIHAlign::Left, GUIVAlign::Bottom, "Host"))
			{
				_hostClientModal = false;
			}
			if (GUIButton::DrawMainMenuButton({ -16,-12 }, { 160,30 }, GUIHAlign::Right, GUIVAlign::Bottom, "Join"))
			{
				_hostClientModal = false;
				_connectIPModal = true;
			}
		}
		GUI::EndLayout();
	}


}

