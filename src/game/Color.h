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


struct Color32 {
	uint32_t value;

	constexpr Color32() :value(0) {}

	constexpr Color32(uint32_t c) : value(c) {}

	constexpr Color32(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
		: value(0)
	{
		value = r | (g << (uint32_t)8) | (b << (uint32_t)16) | (a << (uint32_t)24);
	}

	constexpr explicit Color32(float r, float g, float b, float a)
		:Color32((uint8_t)(r * 255), (uint8_t)(g * 255), (uint8_t)(b * 255), (uint8_t)(a * 255)) {}

	constexpr explicit Color32(const Color& color)
		: Color32(color.r, color.g, color.b, color.a) {}

	constexpr uint8_t GetR() const {
		return (value & 0xFF);
	}
	constexpr uint8_t GetG() const {
		return (value >> 8) & 0xFF;
	}
	constexpr uint8_t GetB() const {
		return (value >> 16) & 0xFF;
	}
	constexpr uint8_t GetA() const {
		return (value >> 24) & 0xFF;
	}
};
namespace Colors {
	constexpr Color Black = { 0,0,0,1 };
	constexpr Color White = { 1,1,1,1 };
	constexpr Color Gray = { 0.5f,0.5f,0.5f,1 };

	constexpr Color LightGreen = { 0.56f, 0.93f,0.56f,1.0f };
	constexpr Color Orange{ 1.0f,0.5f,0.0f,1.0f };
	constexpr Color Red{ 1.0f,0.0f,0.0f,1.0f };
	constexpr Color IndianRed{ 0.8f,0.36f,0.36f,1.0f };
	constexpr Color MediumPurple{ 0.57f,0.43f,0.85f,1.0f };
	constexpr Color LightBlue{ 0.67f,0.84f,0.90f,1.0f };
	constexpr Color Blue{ 0,0,1,1 };
	constexpr Color LightPink{ 1.0f,0.71f, 0.75f, 1.0f };
	constexpr Color CornflowerBlue = Color::ValueOf(0x6495EDFF);
	constexpr Color Transparent = { 0,0,0,0 };

	constexpr Color UIGreen = { 0x10fc18ff };

	constexpr Color SCRed = { 0xf40404ff };
	constexpr Color SCBlue = { 0x0c48ccff };
	constexpr Color SCLightGreen = { 0x2cb494ff };
	constexpr Color SCPurle = { 0x88409cff };
	constexpr Color SCOrange = { 0xf88c14ff };
	constexpr Color SCGreen = { 0x088008ff };
	constexpr Color SCBrown = { 0x703014ff };
	constexpr Color SCLightYellow = { 0xfcfc7cff };
	constexpr Color SCWhite = { 0xcce0d0ff };
	constexpr Color SCTeal = { 0xecc4b0ff };
	constexpr Color SCYellow = { 0xfcfc38ff };
	constexpr Color SCLightBlue = { 0x4068d4ff };

	constexpr Color MapFriendly = { 0x0dff16ff };
	constexpr Color MapResource = { 0x0dff16ff };

}