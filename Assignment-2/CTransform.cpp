#include "CTransform.h"

CTransform::CTransform()
	: pos(0, 0), speed(0, 0), scale(0, 0), angle(0) {}

CTransform::CTransform(const Vec2& pos, const Vec2& velocity)
	: pos(pos), speed(speed), scale(0, 0), angle(0) {}