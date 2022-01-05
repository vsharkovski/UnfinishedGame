#include "Physics.h"

Vec2 Physics::GetOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
	// return overlap rectangle size of the bounding boxes of a and b
	if (!a->hasComponent<CBoundingBox>() || !b->hasComponent<CBoundingBox>())
	{
		return Vec2(0.0f, 0.0f);
	}

	auto& ta = a->getComponent<CTransform>();
	auto& tb = b->getComponent<CTransform>();
	auto& ca = a->getComponent<CBoundingBox>();
	auto& cb = b->getComponent<CBoundingBox>();
	return Vec2(
		ca.halfSize.x + cb.halfSize.x - std::abs(ta.pos.x - tb.pos.x),
		ca.halfSize.y + cb.halfSize.y - std::abs(ta.pos.y - tb.pos.y)
	);
}

Vec2 Physics::GetPreviousOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
	// return overlap rectangle size of previous bounding boxes of a and b
	if (!a->hasComponent<CBoundingBox>() || !b->hasComponent<CBoundingBox>())
	{
		return Vec2(0.0f, 0.0f);
	}

	auto& ta = a->getComponent<CTransform>();
	auto& tb = b->getComponent<CTransform>();
	auto& ca = a->getComponent<CBoundingBox>();
	auto& cb = b->getComponent<CBoundingBox>();
	return Vec2(
		ca.halfSize.x + cb.halfSize.x - std::abs(ta.prevPos.x - tb.prevPos.x),
		ca.halfSize.y + cb.halfSize.y - std::abs(ta.prevPos.y - tb.prevPos.y)
	);
}

bool Physics::IsInside(const Vec2& pos, std::shared_ptr<Entity> e)
{
	// if no animation, cant be inside it
	if (!e->hasComponent<CAnimation>()) { return false; }

	Vec2 halfSize = e->getComponent<CAnimation>().animation.getSize() / 2.0f;
	Vec2 delta = (e->getComponent<CTransform>().pos - pos).abs();
	return (delta.x <= halfSize.x) && (delta.y <= halfSize.y);
}

Physics::Intersect Physics::LineIntersect(const Vec2& a, const Vec2& b, const Vec2& c, const Vec2& d)
{
	// does line ab intersect with line cd?
	Vec2 r = b - a;
	Vec2 s = d - c;
	float rxs = r.crossProduct(s);
	Vec2 cma = c - a;
	float t = cma.crossProduct(s) / rxs;
	float u = cma.crossProduct(r) / rxs;
	if (t >= 0 && t <= 1 && u >= 0 && u <= 1)
	{
		return { true, Vec2(a.x + t * r.x, a.y + t * r.y) };
	}
	return { false, Vec2(0, 0) };
}

bool Physics::EntityIntersect(const Vec2& a, const Vec2& b, std::shared_ptr<Entity> e)
{
	// does line ab intersect with boundingbox of e?
	// (the 4 lines of the boundingbox)
	auto& pos = e->getComponent<CTransform>().pos;
	auto& box = e->getComponent<CBoundingBox>();

	Vec2 p1(pos.x - box.halfSize.x, pos.y - box.halfSize.y);
	Vec2 p2(pos.x + box.halfSize.x, pos.y - box.halfSize.y);
	Vec2 p3(pos.x + box.halfSize.x, pos.y + box.halfSize.y);
	Vec2 p4(pos.x - box.halfSize.x, pos.y + box.halfSize.y);

	if (LineIntersect(a, b, p1, p2).result) return true;
	if (LineIntersect(a, b, p2, p3).result) return true;
	if (LineIntersect(a, b, p3, p4).result) return true;
	if (LineIntersect(a, b, p4, p1).result) return true;

	return false;
}