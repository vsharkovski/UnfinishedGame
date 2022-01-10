#pragma once

#include "Common.h"
#include "Entity.h"
#include "visibility.h"

namespace Physics
{
	Vec2 GetOverlap(Entity a, Entity b)
	{
		// return overlap rectangle size of the bounding boxes of a and b
		if (!a.hasComponent<CBoundingBox>() || !b.hasComponent<CBoundingBox>())
		{
			return Vec2(0.0f, 0.0f);
		}

		auto& ta = a.getComponent<CTransform>();
		auto& tb = b.getComponent<CTransform>();
		auto& ca = a.getComponent<CBoundingBox>();
		auto& cb = b.getComponent<CBoundingBox>();
		return Vec2(
			ca.halfSize.x + cb.halfSize.x - std::abs(ta.pos.x - tb.pos.x),
			ca.halfSize.y + cb.halfSize.y - std::abs(ta.pos.y - tb.pos.y)
		);
	}

	Vec2 GetPreviousOverlap(Entity a, Entity b)
	{
		// return overlap rectangle size of previous bounding boxes of a and b
		if (!a.hasComponent<CBoundingBox>() || !b.hasComponent<CBoundingBox>())
		{
			return Vec2(0.0f, 0.0f);
		}

		auto& ta = a.getComponent<CTransform>();
		auto& tb = b.getComponent<CTransform>();
		auto& ca = a.getComponent<CBoundingBox>();
		auto& cb = b.getComponent<CBoundingBox>();
		return Vec2(
			ca.halfSize.x + cb.halfSize.x - std::abs(ta.prevPos.x - tb.prevPos.x),
			ca.halfSize.y + cb.halfSize.y - std::abs(ta.prevPos.y - tb.prevPos.y)
		);
	}

	bool IsInside(const Vec2& pos, Entity e)
	{
		// if no animation, cant be inside it
		if (!e.hasComponent<CAnimation>()) { return false; }
		return (std::abs(e.getComponent<CTransform>().pos.x - pos.x) <= e.getComponent<CAnimation>().animation.getSize().x / 2.0f)
			&& (std::abs(e.getComponent<CTransform>().pos.y - pos.y) <= e.getComponent<CAnimation>().animation.getSize().y / 2.0f);
	}

	bool LineIntersect(const Vec2& a, const Vec2& b, const Vec2& c, const Vec2& d)
	{
		// does line ab intersect with line cd?
		Vec2 r = b - a;
		Vec2 s = d - c;
		float rxs = cross(r, s);
		Vec2 cma = c - a;
		float t = cross(cma, s) / rxs;
		float u = cross(cma, r) / rxs;
		if (t >= 0 && t <= 1 && u >= 0 && u <= 1)
		{
			return true;
		}
		return false;
	}

	bool EntityIntersect(const Vec2& a, const Vec2& b, Entity e)
	{
		// does line ab intersect with boundingbox of e?
		// (the 4 lines of the boundingbox)
		auto& pos = e.getComponent<CTransform>().pos;
		auto& box = e.getComponent<CBoundingBox>();

		Vec2 p1(pos.x - box.halfSize.x, pos.y - box.halfSize.y);
		Vec2 p2(pos.x + box.halfSize.x, pos.y - box.halfSize.y);
		Vec2 p3(pos.x + box.halfSize.x, pos.y + box.halfSize.y);
		Vec2 p4(pos.x - box.halfSize.x, pos.y + box.halfSize.y);

		if (LineIntersect(a, b, p1, p2)) return true;
		if (LineIntersect(a, b, p2, p3)) return true;
		if (LineIntersect(a, b, p3, p4)) return true;
		if (LineIntersect(a, b, p4, p1)) return true;

		return false;
	}
};
