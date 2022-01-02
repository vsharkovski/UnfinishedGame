#pragma once

#include "Common.h"
#include "Animation.h"
#include "Assets.h"

class Component
{
public:
	bool has = false;
};

class CTransform : public Component
{
public:
	// player max speed specified in level file
	Vec2 pos = { 0.0f, 0.0f };
	Vec2 prevPos = { 0.0f, 0.0f };
	Vec2 scale = { 1.0f, 1.0f };
	Vec2 velocity = { 0.0f, 0.0f };
	float angle = 0.0f;

	CTransform() {}
	CTransform(const Vec2& p)
		: pos(p), prevPos(p) {}
	CTransform(const Vec2& p, const Vec2& v, const Vec2& sc, float a)
		: pos(p), prevPos(p), velocity(v), scale(sc), angle(a) {}
};

class CLifespan : public Component
{
public:
	int lifespan = 0;
	int frameCreated = 0;
	CLifespan() {}
	CLifespan(int duration, int frame)
		: lifespan(duration), frameCreated(frame) {}
};

class CInput : public Component
{
public:
	bool up = false;
	bool down = false;
	bool left = false;
	bool right = false;
	bool shoot = false;
	bool canShoot = false;
	bool canJump = false;
	bool jumping = false;
	CInput() {}
};

class CBoundingBox : public Component
{
public:
	// specified in level file
	// player sprite and bbox centered on player position
	Vec2 size;
	Vec2 halfSize;
	CBoundingBox() {}
	CBoundingBox(const Vec2& s)
		: size(s), halfSize(s.x / 2.0f, s.y / 2.0f) {}
};

class CAnimation : public Component
{
public:
	Animation animation;
	bool repeat = false;
	CAnimation() {}
	CAnimation(const Animation& animation, bool repeat)
		: animation(animation), repeat(repeat) {}
};

class CGravity : public Component
{
public:
	// acceleration, added to velocity
	// when landing, or hitting block with head (block is above), set velocity to 0
	float gravity = 0.0f;
	CGravity() {}
	CGravity(float gravity)
		: gravity(gravity) {}
};

class CState : public Component
{
public:
	// jumping, standing, running
	// assign correct animation from state
	std::string state = "JUMP";
	CState() {}
	CState(const std::string& state)
		: state(state) {}
};

class CDraggable : public Component
{
public:
	bool dragging = false;
	CDraggable() {}
};