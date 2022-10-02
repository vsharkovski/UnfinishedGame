#pragma once

#include "Common.h"
#include "Animation.h"
#include "Assets.h"

class CTransform
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

class CLifespan
{
public:
	int lifespan = 0;
	int frameCreated = 0;
	CLifespan() {}
	CLifespan(int duration, int frame)
		: lifespan(duration), frameCreated(frame) {}
};

class CDamage
{
public:
	int damage = 1;
	CDamage() {}
	CDamage(int damage)
		: damage(damage) {}
};

class CInvincibility
{
public:
	int iframes = 0;
	CInvincibility() {}
	CInvincibility(int iframes)
		: iframes(iframes) {}
};

class CHealth
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

class CInput
{
public:
	bool up = false;
	bool down = false;
	bool left = false;
	bool right = false;
	bool modifier1 = false;
	bool modifier2 = false;
	CInput() {}
};

class CBoundingBox
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

class CAnimation
{
public:
	Animation animation;
	bool repeat = false;
	CAnimation() {}
	CAnimation(const Animation& animation, bool repeat)
		: animation(animation), repeat(repeat) {}
};

class CState
{
public:
	std::string state = "stand";
	CState() {}
	CState(const std::string& state)
		: state(state) {}
};

class CFollowPlayer
{
public:
	Vec2 home = { 0.0f, 0.0f };
	float speed = 0.0f;
	CFollowPlayer() {}
	CFollowPlayer(Vec2 home, float speed)
		: home(home), speed(speed) {}
};

class CPatrol
{
public:
	std::vector<Vec2> positions;
	size_t currentPosition = 0; // the position to walk towards
	float speed = 0.0f;
	CPatrol() {}
	CPatrol(std::vector<Vec2>& positions, float speed)
		: positions(positions), speed(speed) {}
};

class CDraggable
{
public:
	bool dragging = false;
	CDraggable() {}
	CDraggable(bool dragging)
		: dragging(dragging) {}
};

class CClickable
{
public:
	bool leftClicked = false;
	bool rightClicked = false;
	CClickable() {}
	CClickable(bool leftClicked, bool rightClicked)
		: leftClicked(leftClicked), rightClicked(rightClicked) {}
};

class CGuiTemplate
{
public:
	CGuiTemplate() {}
};