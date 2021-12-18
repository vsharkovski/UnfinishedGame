#include "CTransform.h"

CTransform::CTransform()
	: pos(0, 0), velocity(0, 0) {}

CTransform::CTransform(const Vec2& pos, const Vec2& velocity)
	: pos(pos), velocity(velocity) {}