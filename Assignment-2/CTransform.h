#pragma once
#include "Vec2.h"

class CTransform
{
public:
	Vec2 pos;
	Vec2 speed;
	Vec2 scale;
	double angle;

	CTransform();
	CTransform(const Vec2& pos, const Vec2& speed);
};

