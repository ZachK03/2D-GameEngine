#include "Physics.h"
#include "Components.h"
#include <iostream>

Vec2 Physics::GetOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b) const
{
	//Return overlap rectangle size of the bounding boxes of entity a and b
	auto& aBB = a->getComponent<CBoundingBox>();
	auto& bBB = b->getComponent<CBoundingBox>();
	auto& aT = a->getComponent<CTransform>();
	auto& bT = b->getComponent<CTransform>();
	return Vec2((aBB.halfSize.x + bBB.halfSize.x) - std::abs((float)aT.pos.x - bT.pos.x), (aBB.halfSize.y + bBB.halfSize.y) - std::abs((float)aT.pos.y - bT.pos.y));
}

Vec2 Physics::GetPreviousOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b) const
{
	auto& aBB = a->getComponent<CBoundingBox>();
	auto& bBB = b->getComponent<CBoundingBox>();
	auto& aT = a->getComponent<CTransform>();
	auto& bT = b->getComponent<CTransform>();
	return Vec2((aBB.halfSize.x + bBB.halfSize.x) - std::abs((float)aT.prevPos.x - bT.prevPos.x), (aBB.halfSize.y + bBB.halfSize.y) - std::abs((float)aT.prevPos.y - bT.prevPos.y));

	return Vec2(0, 0);
}