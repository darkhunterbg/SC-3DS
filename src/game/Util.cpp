#include "Util.h"
#include "Platform.h"

void Util::DrawTransparentRectangle(const Rectangle& rect, Color c) {
	Vector2Int src = rect.position;
	Vector2Int dst = src + Vector2Int(rect.size.x - 1, 0);
	Platform::DrawLine(src, dst, c);

	dst = src + Vector2Int(0, rect.size.y - 1);
	Platform::DrawLine(src, dst, c);

	src = rect.position + Vector2Int(rect.size.x - 1, 0);
	dst = src + Vector2Int(0, rect.size.y - 1);
	Platform::DrawLine(src, dst, c);

	src = rect.position + Vector2Int(0, rect.size.y - 1);
	dst = src + Vector2Int(rect.size.x - 1, 0);

	Platform::DrawLine(src, dst, c);
}

void Util::DrawTransparentRectangle(const Rectangle16& rect, Color c)
{
	Rectangle dst = { Vector2Int(rect.position), Vector2Int(rect.size) };
	DrawTransparentRectangle(dst, c);
}
