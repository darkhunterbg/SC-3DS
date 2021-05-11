#pragma once

#include <cmath>
#include <stdint.h>

static const constexpr float PI = 3.14159265359f;

/// <summary>
/// Note: this will return 0 if x = 0!
/// </summary>
/// <param name="x"></param>
/// <returns></returns>
constexpr int sign(int x) {
	return (x >> 31) | ((unsigned)-x >> 31);
}

constexpr float RadToDeg(float rad) {
	return (rad * 180.f) / PI;
}

constexpr float DegToRad(float degree) {
	return (degree * PI) / 180.0f;
}

constexpr float ClampF(float x, float min, float max)
{
	return x <= min ? min : x >= max ? max : x;
}


inline static float LerpF(float a, float b, float l)
{
	float c = b - a;
	c *= l;
	c += a;
	return c;
}

template<class T>
struct Vector2T
{
	T x, y;

	constexpr Vector2T() {
		x = 0, y = 0;
	}
	constexpr Vector2T(T x, T y) { this->x = x; this->y = y; }
	Vector2T(const Vector2T& o) { x = o.x, y = o.y; }
	template<class T2>
	constexpr explicit Vector2T(const Vector2T<T2>& a) {
		x = (T)a.x;
		y = (T)a.y;
	}
	constexpr Vector2T(T all) {
		x = all, y = all;
	}

	constexpr inline float Length() const
	{
		return sqrtf(x * x + y * y);
	}

	constexpr inline T LengthSquared() const
	{
		return x * x + y * y;
	}

	constexpr inline unsigned LengthSquaredInt() const
	{
		return x * x + y * y;
	}

	constexpr inline void Normalize()
	{
		float l = Length();
		x /= l;
		y /= l;
	}

	//template<class T2>
	//inline Vector2T& operator =(const Vector2T<T2>& a) {
	//	x = (T)a.x;
	//	y = (T)a.y;

	//	return *this;
	//}


	static Vector2T Normalize(const Vector2T& a)
	{
		Vector2T b = a;
		b.Normalize();
		return b;
	}

	inline bool operator == (const Vector2T& b) const {
		return !(x - b.x) && !(y - b.y);
	}

	inline bool operator != (const Vector2T& b) const {
		return x - b.x || y - b.y;
	}

	inline Vector2T operator+(const Vector2T& b) const
	{
		return Vector2T( x + b.x, y + b.y );
	}
	inline Vector2T operator-(const Vector2T& b) const
	{
		return Vector2T( x - b.x, y - b.y );
	}
	inline Vector2T operator*(const Vector2T& b) const
	{
		return Vector2T( x * b.x, y * b.y );
	}
	inline Vector2T operator/(const Vector2T& b) const
	{
		return Vector2T( x / b.x, y / b.y );
	}
	inline  Vector2T& operator+=(const Vector2T& b)
	{
		x += b.x;
		y += b.y;
		return *this;
	}
	inline Vector2T& operator-=(const Vector2T& b)
	{
		x -= b.x;
		y -= b.y;
		return *this;
	}
	inline  Vector2T& operator*=(const Vector2T& b)
	{
		x *= b.x;
		y *= b.y;
		return *this;
	}
	inline Vector2T& operator/=(const Vector2T& b)
	{
		x /= b.x;
		y /= b.y;
		return *this;
	}

	inline static Vector2T Lerp(const Vector2T<T>& a, const Vector2T& b, float l)
	{
		Vector2T c = b - a;
		c *= l;
		c += a;
		return c;
	}
};


//struct Vector2 : Vector2T<float> {};
//struct Vector2Int : Vector2T<int> {};

typedef Vector2T<int> Vector2Int;
typedef Vector2T<float> Vector2;
typedef Vector2T<int16_t> Vector2Int16;
typedef Vector2T<int8_t> Vector2Int8;

struct RectangleF
{
	Vector2 min;
	Vector2 max;

	inline Vector2 GetSize() const
	{
		return max - min;
	}
	inline void SetSize(Vector2 size)
	{
		Vector2 c = GetCenter();
		min = c - size / 2;
		max = c + size / 2;
	}
	inline Vector2 GetMin() const
	{
		return min;
	}

	inline Vector2 GetMax() const
	{
		return max;
	}

	inline void SetMin(Vector2Int min)
	{
		this->min = Vector2(min);
	}

	inline void SetMax(Vector2Int max)
	{
		this->max = Vector2(max);
	}
	inline Vector2 GetCenter() const
	{
		return min + GetSize() / 2.0f;
	}
	inline void SetCenter(Vector2 center)
	{
		Vector2 s = GetSize();
		min = center + s / -2.0f;
		max = center + s / 2.0f;
	}
	inline bool Intersects(const RectangleF& r) const
	{
		return r.min.x < max.x&& min.x < r.max.x&&
			r.min.y < max.y&& min.y < r.max.y;
	}
	inline void Inflate(Vector2 inflation)
	{
		min -= inflation;
		max += inflation;
	}
};
struct Rectangle
{
	Vector2Int position, size;

	Rectangle() {}
	Rectangle(const Vector2Int& pos, const Vector2Int& s)
		: position(pos), size(s) {}


	inline Vector2Int GetSize() const
	{
		return  size;
	}
	inline void SetSize(Vector2Int size)
	{
		this->size = size;
	}

	inline Vector2Int GetMin() const
	{
		return position;
	}

	inline Vector2Int GetMax() const
	{
		return position + size;
	}

	inline void SetMin(Vector2Int min)
	{
		position = min;
	}

	inline void SetMax(Vector2Int max)
	{
		size = (max - position);
	}

	inline Vector2Int GetCenter() const
	{
		return position + size / 2;
	}
	inline void SetCenter(Vector2Int center)
	{
		position = center;
		position -= size / 2;
	}
	inline bool Intersects(const Rectangle& r) const
	{
		return r.position.x < position.x + size.x && position.x < r.position.x + r.size.x &&
			r.position.y < position.y + size.y && position.y < r.position.y + r.size.y;
	}
	inline bool Contains(const Vector2Int& v) const {
		return v.x <= position.x + size.x && v.x >= position.x &&
			v.y <= position.y + size.y && v.y >= position.y;
	}
};



struct Rectangle16
{
	Vector2Int16 position, size;

	Rectangle16() {}
	Rectangle16(const Vector2Int16& pos, const Vector2Int16& s)
		: position(pos), size(s) {}


	inline Vector2Int16 GetSize() const
	{
		return  size;
	}
	inline void SetSize(Vector2Int16 size)
	{
		this->size = size;
	}

	inline Vector2Int16 GetMin() const
	{
		return position;
	}

	inline Vector2Int16 GetMax() const
	{
		return position + size;
	}

	inline void SetMin(Vector2Int16 min)
	{
		position = min;
	}

	inline void SetMax(Vector2Int16 max)
	{
		size = (max - position);
	}

	inline Vector2Int16 GetCenter() const
	{
		return position + size / 2;
	}
	inline void SetCenter(Vector2Int16 center)
	{
		position = center;
		position -= size / 2;
	}
	inline bool Intersects(const Rectangle16& r) const
	{
		return r.position.x < position.x + size.x && position.x < r.position.x + r.size.x &&
			r.position.y < position.y + size.y && position.y < r.position.y + r.size.y;
	}
	inline bool Contains(const Vector2Int16& v) const {
		return v.x <= position.x + size.x && v.x >= position.x &&
			v.y <= position.y + size.y && v.y >= position.y;
	}

};

inline Vector2Int Vector2Int_Ceil(const Vector2& v) {
	return { (int)ceil(v.x), (int)ceilf(v.y) };
}