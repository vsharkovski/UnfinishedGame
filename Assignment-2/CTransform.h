#pragma once
#include "Vec2.h"

class CTransform
{
public:
	Vec2 pos;
	Vec2 velocity;
	CTransform();
	CTransform(const Vec2& pos, const Vec2& velocity);
};

