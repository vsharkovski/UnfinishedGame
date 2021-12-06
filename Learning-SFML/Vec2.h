#pragma once
class Vec2
{
public:
	float x, y;
	Vec2();
	Vec2(float, float);
	Vec2& add(float);
	Vec2& scale(float);
	float dist(const Vec2&);
	Vec2 operator+(const Vec2&);
	Vec2 operator*(float);
	bool operator==(const Vec2&);
};

