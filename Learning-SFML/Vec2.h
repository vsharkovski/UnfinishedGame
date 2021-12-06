#pragma once
class Vec2
{
public:
	float x, y;
	Vec2();
	Vec2(float, float);
	void add(float);
	void scale(float);
	Vec2 operator+(const Vec2&);
	Vec2 operator*(float);
	bool operator==(const Vec2&);
};

