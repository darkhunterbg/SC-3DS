#include "GUILabel.h"

#include "../Engine/GraphicsRenderer.h"
#include "../Platform.h"
#include "../Game.h"

void GUILabel::DrawText(const Font& font, const char* text, Color color)
{
	Rectangle space = GUI::GetLayoutSpace();

	if (GUI::GetState().TextEffects.test( GUITextEffects_Shadow))
		GraphicsRenderer::DrawText(font, space.position + Vector2Int{ 1, 1 }, text, Colors::Black);
	GraphicsRenderer::DrawText(font, space.position, text, color);
}


void GUILabel::DrawText(const Font& font, const char* text, Vector2Int position, Color color)
{
	GUI::AddNextElementOffset(position);
	DrawText(font, text, color);
}

void GUILabel::DrawText(const Font& font, const char* text,
	GUIHAlign hAlign, GUIVAlign vAlign, Color color)
{
	Vector2Int size = Platform::MeasureString(font, text);
	Rectangle rect = { {0,0}, size };
	Vector2Int pos = GUI::GetPosition(rect, hAlign, vAlign);

	if (GUI::GetState().TextEffects.test(GUITextEffects_Shadow))
		GraphicsRenderer::DrawText(font, pos + Vector2Int{ 1, 1 }, text, Colors::Black);
	GraphicsRenderer::DrawText(font, pos, text, color);


}

void GUILabel::DrawText(const Font& font, const char* text, Vector2Int offset,
	GUIHAlign hAlign, GUIVAlign vAlign, Color color)
{
	GUI::AddNextElementOffset(offset);
	DrawText(font, text, hAlign, vAlign, color);
}

void GUILabel::DrawMenuText( const char* text, Vector2Int offset, GUIHAlign hAlign, GUIVAlign vAlign)
{
	auto font = GUI::GetState().GetFont(Game::SystemFont12);

	GUI::GetState().TextEffects.set(GUITextEffects_Shadow);
	GUILabel::DrawText(*font, text, offset, hAlign, vAlign, Colors::UILightGray);
	GUI::GetState().TextEffects.set(GUITextEffects_None);
}
