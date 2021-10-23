#include "GUILabel.h"

#include "../Engine/GraphicsRenderer.h"
#include "../Platform.h"

void GUILabel::DrawText(const Font& font, const char* text, Vector2Int position, Color color)
{
	GraphicsRenderer::DrawText(font, GUI::GetPosition(position), text, color);
}

void GUILabel::DrawText(const Font& font, const char* text, Vector2Int position,
	GUIHAlign hAlign, GUIVAlign vAlign, Color color)
{
	Vector2Int size = Platform::MeasureString(font, text);
	Rectangle rect = { position, size };
	Vector2Int pos = GUI::GetRelativePosition(rect, hAlign, vAlign);

	GraphicsRenderer::DrawText(font, pos, text, color);
}