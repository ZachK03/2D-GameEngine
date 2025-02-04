#pragma once

#include <map>
#include <memory>

#include "GameEngine.h"
#include "EntityManager.h"

class Scene;
class GameEngine;

class Scene_Play : public Scene
{
	struct PlayerConfig
	{
		float X, Y, CX, CY, SPEED, MAXSPEED, JUMP, GRAVITY;
		std::string WEAPON;
	};

protected:
	std::shared_ptr<Entity> m_player;
	std::string m_levelPath;
	std::string m_levelName;
	PlayerConfig m_playerConfig;
	bool m_drawTextures = true;
	bool m_drawCollision = false;
	bool m_drawGrid = false;
	const Vec2 m_gridSize = { 64, 64 };
	sf::Text m_gridText;
	size_t introTextStop;
	bool gameStarted = false;

	void init(const std::string& levelPath);

	void loadLevel(const std::string& filename);
	virtual void onEnd();

	Vec2 gridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity);
public:
	Scene_Play(GameEngine* gameEngine, const std::string& levelPath);
	void spawnPlayer();
	void spawnBullet(std::shared_ptr<Entity> entity);
	void update();
	void sMovement();
	void sCollision();
	void sDoAction(const Action& action);
	void sAnimation();
	void sLifespan();
	virtual void sRender();
};

