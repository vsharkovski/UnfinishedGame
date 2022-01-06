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
	Vec2 pos = { 0.0f, 0.0f };
	Vec2 prevPos = { 0.0f, 0.0f };
	Vec2 scale = { 1.0f, 1.0f };
	Vec2 velocity = { 0.0f, 0.0f };
	Vec2 facing = { 0.0f, 1.0f };
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

class CDamage : public Component
{
public:
	int damage = 1;
	CDamage() {}
	CDamage(int damage)
		: damage(damage) {}
};

class CInvincibility : public Component
{
public:
	int iframes = 0;
	CInvincibility() {}
	CInvincibility(int iframes)
		: iframes(iframes) {}
};

class CHealth : public Component
{
public:
	int max = 1;
	int current = 1;
	CHealth() {}
	CHealth(int max)
		: max(max), current(max) {}
	CHealth(int max, int current)
		: max(max), current(current) {}
};

class CInput : public Component
{
public:
	bool up = false;
	bool down = false;
	bool left = false;
	bool right = false;
	bool attack = false;
	bool canAttack = false;
	CInput() {}
};

class CBoundingBox : public Component
{
public:
	Vec2 size;
	Vec2 halfSize;
	bool blockMove = false;
	bool blockVision = false;
	CBoundingBox() {}
	CBoundingBox(const Vec2& size)
		: size(size),
		halfSize(size.x / 2.0f, size.y / 2.0f) {}
	CBoundingBox(const Vec2& size, bool blockMove, bool blockVision)
		: size(size),
		halfSize(size.x / 2.0f, size.y / 2.0f),
		blockMove(blockMove),
		blockVision(blockVision) {}
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

class CState : public Component
{
public:
	std::string state = "stand";
	CState() {}
	CState(const std::string& state)
		: state(state) {}
};

class CFollowPlayer : public Component
{
public:
	Vec2 home = { 0.0f, 0.0f };
	float speed = 0.0f;
	CFollowPlayer() {}
	CFollowPlayer(Vec2 home, float speed)
		: home(home), speed(speed) {}
};

class CPatrol : public Component
{
public:
	std::vector<Vec2> positions;
	size_t currentPosition = 0; // the position to walk towards
	float speed = 0.0f;
	CPatrol() {}
	CPatrol(std::vector<Vec2>& positions, float speed)
		: positions(positions), speed(speed) {}
};
