#include "Vec2.h"
#include <cmath>

Vec2::Vec2() {}

Vec2::Vec2(float x, float y)
	: x(x), y(y) {}

bool Vec2::operator == (const Vec2& rhs) const
{
	return x == rhs.x && y == rhs.y;
}

bool Vec2::operator != (const Vec2& rhs) const
{
	return !(*this == rhs);
}

Vec2 Vec2::operator + (const Vec2& rhs) const
{
	return Vec2(x + rhs.x, y + rhs.y);
}

Vec2 Vec2::operator - (const Vec2& rhs) const
{
	return Vec2(x - rhs.x, y - rhs.y);
}

Vec2 Vec2::operator * (const float val) const
{
	return Vec2(x * val, y * val);
}

Vec2 Vec2::operator / (const float val) const
{
	return Vec2(x / val, y / val);
}

void Vec2::operator += (const Vec2& rhs)
{
	x += rhs.x;
	y += rhs.y;
}

void Vec2::operator -= (const Vec2& rhs)
{
	x -= rhs.x;
	y -= rhs.y;
}

void Vec2::operator *= (const float val)
{
	x *= val;
	y *= val;
}

void Vec2::operator /= (const float val)
{
	x /= val;
	y /= val;
}

float Vec2::dist(const Vec2& rhs) const
{
	return sqrt((x - rhs.x) * (x - rhs.x) + (y - rhs.y) * (y - rhs.y));
}

float Vec2::dist() const
{
	return sqrt(x * x + y * y);
}

void Vec2::clampAbsolute(float maxX, float maxY)
{
	if (maxX < 0.0f) maxX *= -1.0f;
	if (maxY < 0.0f) maxY *= -1.0f;
	
	if (x < 0.0f && x < -maxX)
		x = -maxX;
	else if (x > 0.0f && x > maxX)
		x = maxX;

	if (y < 0.0f && y < -maxY)
		y = -maxY;
	else if (y > 0.0f && y > maxY)
		y = maxY;
}