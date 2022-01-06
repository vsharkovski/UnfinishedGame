#pragma once
#include <cmath>

class Vec2
{
public:
	float x = 0.0f;
	float y = 0.0f;

	Vec2() {}

	Vec2(int x, int y)
		: x(static_cast<float>(x)), y(static_cast<float>(y)) {}

	Vec2(unsigned int x, unsigned int y)
		: x(static_cast<float>(x)), y(static_cast<float>(y)) {}

	Vec2(float x, float y)
		: x(x), y(y) {}

	inline bool operator == (const Vec2& rhs) const
	{
		return x == rhs.x && y == rhs.y;
	}

	inline bool operator != (const Vec2& rhs) const
	{
		return !(*this == rhs);
	}

	inline Vec2 operator + (const Vec2& rhs) const
	{
		return Vec2(x + rhs.x, y + rhs.y);
	}

	inline Vec2 operator - (const Vec2& rhs) const
	{
		return Vec2(x - rhs.x, y - rhs.y);
	}

	inline Vec2 operator * (const Vec2& rhs) const
	{
		return Vec2(x * rhs.x, y * rhs.y);
	}

	inline Vec2 operator / (const Vec2& rhs) const
	{
		return Vec2(x / rhs.x, y / rhs.y);
	}

	inline Vec2 operator * (const float val) const
	{
		return Vec2(x * val, y * val);
	}

	inline Vec2 operator / (const float val) const
	{
		return Vec2(x / val, y / val);
	}

	inline void operator += (const Vec2& rhs)
	{
		x += rhs.x;
		y += rhs.y;
	}

	inline void operator -= (const Vec2& rhs)
	{
		x -= rhs.x;
		y -= rhs.y;
	}

	inline void operator *= (const Vec2& rhs)
	{
		x *= rhs.x;
		y *= rhs.y;
	}

	inline void operator /= (const Vec2& rhs)
	{
		x /= rhs.x;
		y /= rhs.y;
	}

	inline void operator *= (const float val)
	{
		x *= val;
		y *= val;
	}

	inline void operator /= (const float val)
	{
		x /= val;
		y /= val;
	}

	inline float distSq(const Vec2& rhs) const
	{
		return (x - rhs.x) * (x - rhs.x) + (y - rhs.y) * (y - rhs.y);
	}

	inline float dist(const Vec2& rhs) const
	{
		return sqrt(distSq(rhs));
	}

	inline float length() const
	{
		return sqrt(x * x + y * y);
	}

	inline Vec2 normalize() const
	{
		float l = length();
		return Vec2(x / l, y / l);
	}

	inline Vec2 abs() const
	{
		return Vec2(x < 0.0f ? -x : x, y < 0.0f ? -y : y);
	}

	inline Vec2 clampAbsolute(float maxX, float maxY) const
	{
		if (maxX < 0.0f) maxX *= -1.0f;
		if (maxY < 0.0f) maxY *= -1.0f;

		Vec2 res(x, y);
		if (x < 0.0f && x < -maxX)
			res.x = -maxX;
		else if (x > 0.0f && x > maxX)
			res.x = maxX;

		if (y < 0.0f && y < -maxY)
			res.y = -maxY;
		else if (y > 0.0f && y > maxY)
			res.y = maxY;

		return res;
	}

	inline float crossProduct(const Vec2 rhs) const
	{
		return x * rhs.y - y * rhs.x;
	}
};

