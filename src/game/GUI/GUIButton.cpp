#include "GUIButton.h"

#include "GUIImage.h"
#include "GUILabel.h"

#include "../Game.h"
#include "../Color.h"

#include "../Engine/AudioManager.h"
#include "../Data/GameDatabase.h"

bool GUIButton::DrawMainMenuButtonTextOffset(Vector2Int offset, const char* text, bool enabled)
{
	Color c = Colors::UIMenuDisabled;

	if (enabled)
		c = GUI::IsLayoutFocused() ? Colors::UIMenuGreenLit : Colors::UIMenuGreen;

	GUILabel::DrawText(*Game::MenuFont16, text, offset, GUIHAlign::Center, GUIVAlign::Center, c);

	if (enabled)
	{
		if (GUI::OnLayoutFocused())
		{
			Game::PlayUISound(GameDatabase::instance->GetSoundSet("Misc\\ButtonHover"));
		}

		if (GUI::OnLayoutActivated())
		{
			Game::PlayUISound(GameDatabase::instance->GetSoundSet("Misc\\Button2"));
			return true;
		}
	}

	return false;
}



bool GUIButton::DrawMainMenuButton(const char* text, bool enabled)
{
	return DrawMainMenuButtonTextOffset({ 0,0 }, text, enabled);

}

bool GUIButton::DrawMainMenuButton(Rectangle dst, GUIHAlign hAlign, GUIVAlign vAlign, const char* text, bool enabled)
{
	bool result = false;

	GUI::BeginRelativeLayout(dst, hAlign, vAlign);
	{
		result = DrawMainMenuButton(text, enabled);
	}
	GUI::EndLayout();

	return result;
}
