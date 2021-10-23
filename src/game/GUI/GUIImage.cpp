#include "GUIImage.h"
#include "../Engine/GraphicsRenderer.h"
#include "../Platform.h"

void GUIImage::DrawTexture(const Texture& texture, Rectangle rect, Color color)
{
	GraphicsRenderer::Draw(texture, GUI::GetPosition(rect.position), rect.size, Color32(color));
}

void GUIImage::DrawTexture(const Texture& texture, Color color)
{
	Rectangle rect = GUI::GetLayoutSpace();
	GraphicsRenderer::Draw(texture, rect.position, rect.size, Color32(color));
}
void GUIImage::DrawSubTexture(const Texture& texture, Rectangle subImage, Color color)
{
	Rectangle rect = GUI::GetLayoutSpace();
	ImageFrame imgFrame;
	imgFrame.texture = &texture;
	imgFrame.offset = Vector2Int16(subImage.position);
	imgFrame.size = Vector2Int16(subImage.size);
	imgFrame.uv = Platform::GenerateUV(texture.GetTextureId(), Rectangle_ToRectangle16(subImage));

	GraphicsRenderer::Draw(imgFrame, rect, color);
}

void GUIImage::DrawImage(const Image& image, unsigned subImage, Color color)
{
	const ImageFrame& frame = image.GetFrame(subImage);
	DrawImageFrame(frame, color);
}

void GUIImage::DrawImageFrame(const ImageFrame& frame, Color color )
{
	Rectangle rect = GUI::GetLayoutSpace();
	GraphicsRenderer::Draw(frame, rect, color);
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
