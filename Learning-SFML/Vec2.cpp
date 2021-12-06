#include "Vec2.h"

Vec2::Vec2()
	: x(0.0f), y(0.0f) {}

Vec2::Vec2(float xin, float yin)
	: x(xin), y(yin) {}

void Vec2::add(float val)				{ x += val; y += val; }

void Vec2::scale(float scale)			{ x *= scale; y *= scale; }

Vec2 Vec2::operator+ (const Vec2& rhs)	{ return Vec2(x + rhs.x, y + rhs.y); }

Vec2 Vec2::operator* (float scale)		{ return Vec2(x * scale, y * scale); }

bool Vec2::operator== (const Vec2& rhs)	{ return x == rhs.x && y == rhs.y; }