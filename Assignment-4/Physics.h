#pragma once

#include "Common.h"
#include "Entity.h"

namespace Physics
{
	struct Intersect
	{
		bool result;
		Vec2 pos;
	};

	Vec2 GetOverlap(Entity a, Entity b);
	Vec2 GetPreviousOverlap(Entity a, Entity b);
	bool IsInside(const Vec2& pos, Entity e);
	Intersect LineIntersect(const Vec2& a, const Vec2& b, const Vec2& c, const Vec2& d);
	bool EntityIntersect(const Vec2& a, const Vec2& b, Entity e);
};
