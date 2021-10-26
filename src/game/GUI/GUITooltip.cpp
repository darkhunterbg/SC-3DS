#include "GUITooltip.h"
#include "GUIImage.h"
#include "GUILabel.h"
#include "../Engine/GraphicsRenderer.h"
#include "../Platform.h"

struct TooltipData {
	Rectangle dst;
	std::string text;
	const Font* font;
};

void GUITooltip::DrawTextTooltip(const char* id, const Font& font, const char* text)
{
	std::string key = id;
	key += ".TooltipData";

	TooltipData* data = GUI::GetResourceById<TooltipData>(key);
	if (data == nullptr)
	{
		data = new	TooltipData();
		GUI::RegisterResource(key, data, [](void* p) {delete (TooltipData*)p;  });
	}


	Vector2Int size = Platform::MeasureString(font, text);
	Rectangle rect = GUI::GetLayoutSpace();
	data->dst = { { rect.GetMax().x, rect.position.y }, size };
	data->dst.size += {4, 0};
	data->font = &font;
	data->text = text;

	GUI::DrawAtEndOfFrame([data]() {
		GUI::BeginAbsoluteLayout(data->dst);
		GUIImage::DrawColor(Colors::Black);
		GUIImage::DrawBorder(1, Colors::UIBlue);
		GUILabel::DrawText(*data->font, data->text.data(), Vector2Int{ 2,0 }, Colors::UILightGray);
		GUI::EndLayout();
		});
}


