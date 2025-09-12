#pragma once

#include "Vec2.hpp"
#include "SFML/Graphics.hpp"
#include "Animation.hpp"

class Component
{
public:
	bool exists = false;
};

class CTransform : public Component
{
public:
	Vec2f pos = { 0.0f, 0.0f };
	Vec2f prevPos = { 0.0f, 0.0f };
	Vec2f scale = { 1.0f, 1.0f };
	Vec2f velocity = { 0.0f, 0.0f };
	float angle = 0.0f;

	CTransform() = default;
	CTransform(const Vec2f& p) : pos(p) {}
	CTransform(const Vec2f& p, const Vec2f& sp, const Vec2f& sc, float a) : pos(p), prevPos(p), velocity(sp), scale(sc), angle(a) {}
};

class CShape : public Component
{
public:
	sf::CircleShape circle;

	CShape() = default;
	CShape(float radius, size_t points, const sf::Color& fill, const sf::Color& outline, float thickness) : circle(radius, points)
	{
		circle.setFillColor(fill);
		circle.setOutlineColor(outline);
		circle.setOutlineThickness(thickness);
		circle.setOrigin({ radius, radius });
	}
};

class CCollision : public Component
{
public:
	float radius = 0;

	CCollision() = default;
	CCollision(float r) : radius(r) {}
};

class CScore : public Component
{
public:
	int score = 0;

	CScore() = default;
	CScore(int s) : score(s) {}
};

class CLifespan : public Component
{
public:
	int lifespan = 0;
	int frameCreated = 0;

	CLifespan() = default;
	CLifespan(int duration, int frame) : lifespan(duration), frameCreated(frame) {}
};

class CInput : public Component
{
public:
	bool up = false;
	bool down = false;
	bool right = false;
	bool left = false;
	bool shoot = false;
	bool canShoot = true;
	bool canJump = true;

	CInput() = default;
};

class CBoundingBox : public Component
{
public:
	Vec2f size;
	Vec2f halfSize;

	CBoundingBox() = default;
	CBoundingBox(const Vec2f& s) : size(s), halfSize(s.x / 2, s.y / 2) {}
};

class CGravity : public Component
{
public:
	float gravity = 0;

	CGravity() = default;
	CGravity(float g) : gravity(g) {}
};

class CState : public Component
{
public:
	std::string state = "jumping";

	CState() = default;
	CState(const std::string& s) : state(s) {}
};

class CHealth : public Component
{
public:
	int maxHealth;
	int currentHealth;

	CHealth() = default;
	CHealth(int health) : maxHealth(health), currentHealth(health) {}
};

struct Ability
{
	std::string name = "Default";
	bool canUse = true;
	int damage = 0;
	int level = 1;
	float cooldown = 0.0f;
	float lastUsed = 0.0f;
};

class CAbility : public Component
{
public:
	std::vector<Ability> abilities;

	CAbility() = default;
};

class CAnimation : public Component
{
public:
	std::unique_ptr<Animation> animation;

	CAnimation() = default;

	CAnimation(std::unique_ptr<Animation> anim)
		: animation(std::move(anim)) {
	}
};