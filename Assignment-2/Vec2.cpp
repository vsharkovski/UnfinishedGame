#include "Vec2.h"
#include <cmath>

Vec2::Vec2()
	: x(0.0f), y(0.0f) {}

Vec2::Vec2(float xin, float yin)
	: x(xin), y(yin) {}

Vec2::Vec2(const Vec2& rhs)
	: x(rhs.x), y(rhs.y) {}

Vec2& Vec2::operator= (const Vec2& rhs)
{
	x = rhs.x;
	y = rhs.y;
	return *this;
}

Vec2& Vec2::add(float val)
{
	x += val;
	y += val;
	return *this;
}

Vec2& Vec2::scale(float scale)
{
	x *= scale;
	y *= scale;
	return *this;
}

Vec2 Vec2::operator+ (const Vec2& rhs)
{
	return Vec2(x + rhs.x, y + rhs.y);
}

Vec2 Vec2::operator* (float scale)
{
	return Vec2(x * scale, y * scale);
}

bool Vec2::operator== (const Vec2& rhs)
{
	return x == rhs.x && y == rhs.y;
}

float Vec2::dist(const Vec2& rhs)
{
	return sqrt((x - rhs.x) * (x - rhs.x) + (y - rhs.y) * (y - rhs.y));
}