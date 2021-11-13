#pragma once

#include "GUIBase.h"

class GUIImage {
private:
	GUIImage() = delete;
	~GUIImage() = delete;
public:
	static void DrawTextureAbsolute(const Texture& texture, Rectangle rect, Color color = Colors::White);
	static void DrawTexture(const Texture& texture, Color color = Colors::White);

	static void DrawSubTexture(const Texture& texture, Rectangle subImage, Color color = Colors::White);
	static void DrawImage(const Image& image, unsigned subImage = 0, Color color = Colors::White);
	static void DrawImageFrame(const ImageFrame& frame, Color color = Colors::White);
	static void DrawImageFrame( const ImageFrame& frame, Vector2Int offset, Color color = Colors::White);

	static void DrawColor(Color color);
	static void DrawColorAbsolute(Rectangle rect, Color color);

	static void DrawAnimatedImage(const char* id, const Image& image,int* frameState, Color color = Colors::White);

	static void DrawBorder(int bordeThickness, Color color);
};