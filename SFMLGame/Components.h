#pragma once

#include "Animation.h"
#include "Assets.h"


class Component
{
public:
	bool has = false;

	Component() {}
};

class CTransform : public Component {
public:
	Vec2 pos = { 0.0, 0.0 };
	Vec2 prevPos = { 0.0, 0.0 };
	Vec2 scale = { 1.0, 1.0 };
	Vec2 velocity = { 0.0, 0.0 };
	Vec2 initialPos = { 0.0, 0.0 };
	float angle = 0;

	CTransform() {}
	CTransform(const Vec2& p)
		: pos(p), initialPos(p) {}
	CTransform(const Vec2& p, const Vec2& v, const Vec2& sc, float a)
		: pos{ p }, initialPos(p), prevPos(p), velocity{ v }, scale{sc}, angle {a} {}
};

class CShape : public Component {
public:
	sf::CircleShape circle;

	CShape() {}
	CShape(float radius, int points, const sf::Color& fill, const sf::Color& outline, float thickness)
		: circle(radius, points)
	{
		circle.setFillColor(fill);
		circle.setOutlineColor(outline);
		circle.setOutlineThickness(thickness);
		circle.setOrigin(radius, radius);
	}
};

class CCollision : public Component {
public:
	float radius = 0;
	CCollision() {}
	CCollision(float r)
		: radius{ r } {}
};

class CScore : public Component {
public:
	int score = 0;
	CScore() {}
	CScore(int s)
		: score{ s } {}
};

class CLifespan : public Component {
public:
	int lifespan = 0;
	int frameCreated = 0;
	CLifespan() {}
	CLifespan(int duration, int frame)
		: lifespan{ duration }, frameCreated{ frame } {}
};

class CInput : public Component {
public: 
	bool up		= false;
	bool down	= false;
	bool left	= false;
	bool right	= false;
	bool shoot	= false;
	bool canShoot = true;
	bool canJump = true;

	CInput() {}
};

class CBoundingBox : public Component {
public:
	Vec2 size;
	Vec2 halfSize;

	CBoundingBox() {}
	CBoundingBox(const Vec2& s)
		: size(s), halfSize(s.x / 2, s.y / 2) {}
};
class CAnimation : public Component {
public:
	Animation animation;
	bool repeat = false;
	CAnimation() {}
	CAnimation(const Animation& anim, bool r)
		: animation(anim), repeat(r) {}
};
class CGravity : public Component {
public:
	float gravity = 0.0;
	float mass = 10.0;

	CGravity() {}
	CGravity(float g)
		: gravity(g) {}
	CGravity(float g, float m)
		: gravity(g), mass(m) {}
};
class CState : public Component {
public:
	bool isTouchingGround = false;
	std::string state = "jumping";
	CState() {}
	CState(const std::string& s) : state(s) {}
};