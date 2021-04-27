#pragma once

#include <stdint.h>

struct Color
{
	float r, g, b, a;

	Color() {
		r = g = b = a = 0;
	}
	constexpr Color(float r, float g, float b, float a) :r(r), g(g), b(b), a(a) {}
	constexpr Color(uint32_t rgba8888) :
		r(((rgba8888 >> 24) & 0xFF) / 255.f),
		g(((rgba8888 >> 16) & 0xFF) / 255.f),
		b(((rgba8888 >> 8) & 0xFF) / 255.f),
		a(((rgba8888) & 0xFF) / 255.f) {}

	static constexpr Color ValueOf(uint32_t rgba8888) {
		float r = ((rgba8888 >> 24) & 0xFF) / 255.f;
		float g = ((rgba8888 >> 16) & 0xFF) / 255.f;
		float b = ((rgba8888 >> 8) & 0xFF) / 255.f;
		float a = (rgba8888 & 0xFF) / 255.f;

		return Color(r, g, b, a);
	}

	static Color Lerp(const Color& a, const Color& b, float l)
	{
		Color c = { b.r - a.r,b.g - a.g,b.b - a.b,b.a - a.a };
		c.r *= l;
		c.g *= l;
		c.b *= l;
		c.a *= l;

		c.r += a.r;
		c.g += a.g;
		c.b += a.b;
		c.a += a.a;

		return c;
	}


	inline bool operator == (const Color& c) const {
		return r == c.r && g == c.g && b == c.b && a == c.a;
	}

	inline bool operator != (const Color& c) const {
		return r != c.r || g != c.g || b != c.b || a != c.a;
	}
};

namespace Colors {
	constexpr Color Black = { 0,0,0,1 };
	constexpr Color White = { 1,1,1,1 };

	constexpr Color LightGreen = { 0.56f, 0.93f,0.56f,1.0f };
	constexpr Color Orange{ 1.0f,0.5f,0.0f,1.0f };
	constexpr Color Red{ 1.0f,0.0f,0.0f,1.0f };
	constexpr Color IndianRed{ 0.8f,0.36f,0.36f,1.0f };
	constexpr Color MediumPurple{ 0.57f,0.43f,0.85f,1.0f };
	constexpr Color LightBlue{ 0.67f,0.84f,0.90f,1.0f };
	constexpr Color LightPink{ 1.0f,0.71f, 0.75f, 1.0f };
	constexpr Color CornflowerBlue = Color::ValueOf(0x6495EDFF);

}