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

bool Physics::IsInside(Vec2 pos, std::shared_ptr<Entity> e)
{
	// if no animation, cant be inside it
	if (!e->hasComponent<CAnimation>()) { return false; }
	
	Vec2 halfSize = e->getComponent<CAnimation>().animation.getSize() / 2.0f;
	Vec2 delta = (e->getComponent<CTransform>().pos - pos).abs();
	return (delta.x <= halfSize.x) && (delta.y <= halfSize.y);
}