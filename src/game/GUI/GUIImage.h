#pragma once

#include "GUIBase.h"

class GUIImage {
private:
	GUIImage() = delete;
	~GUIImage() = delete;
public:
	static void DrawTexture(const Texture& texture, Rectangle rect, Color color = Colors::White);
	static void DrawTexture(const Texture& texture, Color color = Colors::White);

	static void DrawSubTexture(const Texture& texture, Rectangle subImage, Color color = Colors::White);
	static void DrawImage(const Image& image, unsigned subImage = 0, Color color = Colors::White);
	static void DrawImageFrame(const ImageFrame& frame, Color color = Colors::White);

	static void DrawColor(Color color);
	static void DrawColor(Rectangle rect, Color color);
};