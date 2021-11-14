#include "GUITextbox.h"
#include "GUILabel.h"
#include "../StringLib.h"

#include "../Engine/InputManager.h"

struct TextboxData {
	int frame = 0;
};

static constexpr const int CursorBlinkTime = 30;

bool GUITextbox::DrawTextbox(const char* id, const Font& font, Span<char> text, Color color)
{
	std::string key = id;
	key += ".TextboxData";

	TextboxData* data = GUI::GetResourceById<TextboxData>(key);
	if (data == nullptr)
	{
		data = new	TextboxData();
		GUI::RegisterResource(key, data, [](void* p) {delete (TextboxData*)p;  });

	}

	if (InputManager::TextInput.size() > 0)
	{
		int cursor = std::strlen(text.Data());

		for (char c : InputManager::TextInput)
		{
			if (cursor == text.Size() - 1)
				break;
			text.Data()[cursor++] = c;
		}
		text.Data()[cursor] = 0;
	}
	if (InputManager::TextDelete)
	{
		int cursor = std::strlen(text.Data());
		cursor -= InputManager::TextDelete;
		if (cursor < 0) cursor = 0;

		text.Data()[cursor] = 0;
	}

	Vector2Int offset = { 0,0 };

	if (text.Size() > 0)
	{
		GUILabel::DrawText(font, text.Data(), GUIHAlign::Left, GUIVAlign::Top, color);
		offset += font.MeasureString(text.Data());
		offset.y -= font.MeasureString(" ").y;
	}

	if ((data->frame / CursorBlinkTime) % 2 == 0)
		GUILabel::DrawText(font, "_", offset, GUIHAlign::Left, GUIVAlign::Top, color);

	++data->frame;
	return false;
}
