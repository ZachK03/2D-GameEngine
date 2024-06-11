#pragma once

#include "Entity.h"

class Physics
{
public:
	Vec2 GetOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b) const;
	Vec2 GetPreviousOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b) const;
};