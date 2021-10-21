#pragma once

#include "GUIBase.h"

class GUIImage {
private:
	GUIImage() = delete;
	~GUIImage() = delete;
public:
	static void DrawTexture(const Texture& texture, Rectangle rect, Color color = Colors::White);
	static void DrawTexture(const Texture& texture, Color color = Colors::White);

	static void DrawColor(Color color);
	static void DrawColor(Rectangle rect, Color color);
};