#include "GUIImage.h"
#include "../Engine/GraphicsRenderer.h"

void GUIImage::DrawTexture(const Texture& texture, Rectangle rect, Color color)
{
	GraphicsRenderer::Draw(texture, GUI::GetPosition(rect.position), rect.size, Color32(color));
}

void GUIImage::DrawTexture(const Texture& texture, Color color)
{
	Rectangle rect = GUI::GetLayoutSpace();
	GraphicsRenderer::Draw(texture, rect.position, rect.size,  Color32(color));
}

void GUIImage::DrawColor(Color color)
{
	Rectangle rect = GUI::GetLayoutSpace();
	GraphicsRenderer::DrawRectangle({ GUI::GetPosition(rect.position), rect.size }, color);
}

void GUIImage::DrawColor(Rectangle rect, Color color)
{
	GraphicsRenderer::DrawRectangle({ GUI::GetPosition(rect.position), rect.size }, color);
}
