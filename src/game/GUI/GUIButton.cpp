#include "GUIButton.h"

#include "GUIImage.h"
#include "GUILabel.h"

#include "../Game.h"
#include "../Color.h"

#include "../Engine/AudioManager.h"
#include "../Data/GameDatabase.h"


void GUIButton::SetNextButtonSelected()
{
	GUI::GetState().SelectNextButton = true;
}

bool GUIButton::DrawMainMenuButtonFromText(Vector2Int offset, GUIHAlign hAlign, GUIVAlign vAlign, const char* text, bool enabled)
{
	Vector2Int size = Game::MenuFont16->MeasureString(text);

	GUI::BeginRelativeLayout(offset, size, hAlign, vAlign);
	bool clicked = DrawMainMenuButton(text, enabled);
	GUI::EndLayout();

	return clicked;
}

bool GUIButton::DrawMainMenuButtonTextOffsetAligned(Vector2Int offset, GUIHAlign hAlign, GUIVAlign vAlign, const char* text, bool enabled)
{
	Color c = Colors::UIMenuDisabled;

	bool select = GUI::GetState().SelectNextButton;
	if (!select)
	{
		select = GUI::IsLayoutFocused();
	}
	else
	{
		GUI::GetState().SelectNextButton = false;
	}

	if (enabled)
		c = select ? Colors::UIMenuGreenLit : Colors::UIMenuGreen;

	GUILabel::DrawText(*Game::MenuFont16, text, offset, hAlign, vAlign, c);

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

bool GUIButton::DrawMainMenuButtonTextOffset(Vector2Int offset, const char* text, bool enabled)
{
	return DrawMainMenuButtonTextOffsetAligned(offset, GUIHAlign::Center, GUIVAlign::Center, text, enabled);
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
