#include "Util.h"
#include "Platform.h"
#include "Profiler.h"

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

void Util::DrawTransparentRectangle(const Rectangle& rect, int thickness, Color c) {

	Color32 color = Color32(c);

	Rectangle dst = rect;
	dst.size = Vector2Int(rect.size.x, thickness);
	Platform::DrawRectangle(dst, color);

	dst.position += Vector2Int(0, rect.size.y - thickness);
	Platform::DrawRectangle(dst, color);

	dst = rect;
	dst.size = Vector2Int(thickness, rect.size.y);
	Platform::DrawRectangle(dst, color);

	dst.position += Vector2Int(rect.size.x - thickness, 0);
	Platform::DrawRectangle(dst, color);

	//dst = src + Vector2Int(0, rect.size.y - 1);
	//Platform::DrawLine(src, dst, c);

	//src = rect.position + Vector2Int(rect.size.x - 1, 0);
	//dst = src + Vector2Int(0, rect.size.y - 1);
	//Platform::DrawLine(src, dst, c);

	//src = rect.position + Vector2Int(0, rect.size.y - 1);
	//dst = src + Vector2Int(rect.size.x - 1, 0);

	//Platform::DrawLine(src, dst, c);
}


void Util::DrawTransparentRectangle(const Rectangle16& rect, Color c)
{
	Rectangle dst = { Vector2Int(rect.position), Vector2Int(rect.size) };
	DrawTransparentRectangle(dst, c);
}


void Util::DrawTransparentRectangle(const Rectangle16& rect, int thickness, Color c) {
	Rectangle dst = { Vector2Int(rect.position), Vector2Int(rect.size) };
	DrawTransparentRectangle(dst, thickness, c);
}

void Util::RealTimeStat(const char* stat, int value)
{
	Profiler::AddCounter(stat, value);
}
