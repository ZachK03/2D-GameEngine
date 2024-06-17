#include "Scene_Play.h"
#include "Scene.h"
#include "Physics.h"
#include "Assets.h"
#include "GameEngine.h"
#include "Components.h"

#include <cmath>
#include <iostream>

Scene_Play::Scene_Play(GameEngine* gameEngine, const std::string& levelPath)
	: Scene(gameEngine)
	, m_levelPath(levelPath)
{
	init(m_levelPath);
}

void Scene_Play::init(const std::string& levelPath)
{
	registerAction(sf::Keyboard::P, "PAUSE");
	registerAction(sf::Keyboard::Escape, "QUIT");
	registerAction(sf::Keyboard::T, "TOGGLE_TEXTURE");
	registerAction(sf::Keyboard::C, "TOGGLE_COLLISION");
	registerAction(sf::Keyboard::G, "TOGGLE_GRID");
	registerAction(sf::Keyboard::W, "UP");
	registerAction(sf::Keyboard::A, "LEFT");
	registerAction(sf::Keyboard::D, "RIGHT");
	registerAction(sf::Keyboard::S, "DOWN");
	registerAction(sf::Keyboard::Space, "STOP");
	registerAction(sf::Keyboard::R, "RESET");

	m_gridText.setCharacterSize(12);
	//m_gridText.setFont(m_game->assets().getFont("Tech"));

	loadLevel(levelPath);
}

Vec2 Scene_Play::gridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity)
{
	auto& anim = entity->getComponent<CAnimation>();
	Vec2 animSize = anim.animation.getSize();
	Vec2 pixelPosition;
	pixelPosition.x = m_gridSize.x * gridX + (animSize.x / 2);
	pixelPosition.y = m_game->window().getSize().y - (m_gridSize.y * gridY + (animSize.y / 2));
	//Return Vec2 indicating CENTER of where the entity should be
	//Animation size center
	//m_gridSize.x and m_gridSize.y
	//Bottom left should be in the bottom left of the gridX and gridY
	return pixelPosition;
}

void Scene_Play::loadLevel(const std::string& filename)
{
	//Reset entity manager when a new level is loaded
	m_entityManager = EntityManager();

	//TODO : Read level file and add the appropriate entities
	//Use playerConfig struct to store player properties
	//Defined in Scene_Play.h

	spawnPlayer();

	for (int i = 0; i < 10; i++)
	{
		auto ground = m_entityManager.addEntity("tile");
		ground->addComponent<CAnimation>(m_game->assets().getAnimation("Ground"), true);
		ground->addComponent<CTransform>(gridToMidPixel(i,1,ground));
		ground->addComponent<CBoundingBox>(Vec2(64, 64));
	}

	for (int i = 3; i < 7; i++)
	{
		auto brick = m_entityManager.addEntity("tile");
		brick->addComponent<CAnimation>(m_game->assets().getAnimation("Brick"), true);
		brick->addComponent<CTransform>(gridToMidPixel(i, 4, brick));
		brick->addComponent<CBoundingBox>(Vec2(64, 64));
	}
	
	//if (brick->getComponent<CAnimation>().animation.getName() == "Brick") {} Detect brick entity
}

void Scene_Play::spawnPlayer()
{
	m_player = m_entityManager.addEntity("player");
	m_player->addComponent<CAnimation>(m_game->assets().getAnimation("Stand"), true);
	m_player->addComponent<CTransform>(gridToMidPixel(1, 3, m_player));
	m_player->addComponent<CBoundingBox>(Vec2(48, 48));
	m_player->addComponent<CInput>();
	m_player->addComponent<CState>("state");
	m_player->addComponent<CGravity>(0.2f);
	//m_player->addComponent<CGravity>(9.8f, 62.0f); WITH MASS
}

void Scene_Play::spawnBullet(std::shared_ptr<Entity> entity)
{

}

void Scene_Play::update()
{
	m_entityManager.update();
	if (!m_paused)
	{
		sMovement();
		sLifespan();
		sCollision();
	}
	sAnimation();
	sRender();
}

void Scene_Play::sMovement()
{
	auto& input = m_player->getComponent<CInput>();
	Vec2 vel = { 0.0, m_player->getComponent<CTransform>().velocity.y };
	if (input.up && input.canJump) vel.y += -1.5f;
	//if (input.down) vel.y += 1.0f;
	if (input.left) vel.x += -3.5f;
	if (input.right) vel.x += 3.5f;

	auto& transform = m_player->getComponent<CTransform>();
	transform.velocity = vel;
	m_player->getComponent<CState>().state = "stand";

	if (std::abs(transform.velocity.x) > 0)
		m_player->getComponent<CState>().state = "running";

	if(std::abs(transform.velocity.y) > 0)
		m_player->getComponent<CState>().state = "falling";

	for (auto e : m_entityManager.getEntities())
	{
		if (e->hasComponent<CTransform>())
		{
			auto& eTrans = e->getComponent<CTransform>();
			if (e->hasComponent<CGravity>())
			{
				if (e->hasComponent<CState>())
				{
					if (!e->getComponent<CState>().isTouchingGround)
						eTrans.velocity.y += e->getComponent<CGravity>().gravity;
				}
				else
				{
					eTrans.velocity.y += e->getComponent<CGravity>().gravity;
				}
			}
			if (e->hasComponent<CState>() && eTrans.velocity.y < 0)
				e->getComponent<CState>().isTouchingGround = false;
			if (eTrans.velocity.x > 9.0)
				eTrans.velocity.x = 9.0;
			if (eTrans.velocity.y > 9.0)
				eTrans.velocity.y = 9.0;
			if (eTrans.velocity.x < -9.0)
				eTrans.velocity.x = -9.0;
			if (eTrans.velocity.y < -9.0) 
			{
				eTrans.velocity.y = -9.0;
				input.canJump = false;
			}
			eTrans.prevPos = eTrans.pos;
			eTrans.pos += eTrans.velocity;
			if (eTrans.pos.x - e->getComponent<CBoundingBox>().halfSize.x < 0)
			{
				eTrans.pos.x = e->getComponent<CBoundingBox>().halfSize.x;
			}
			if (eTrans.velocity.x < 0) {
				eTrans.scale.x = -1.0f;
			} else if(eTrans.velocity.x > 0) {
				eTrans.scale.x = 1.0f;
			}
		}
	}

	if (transform.pos.y > m_game->window().getSize().y)
	{
		transform.pos = transform.initialPos;
	}

}

void Scene_Play::sCollision()
{
	bool playerTouchingGround = false;

	//Tile collision
	for (auto e : m_entityManager.getEntities())
	{
		if (e->hasComponent<CBoundingBox>())
		{
			if (e != m_player)
			{
				Vec2 overlap = m_game->physics().GetOverlap(m_player, e);
				auto& transform = m_player->getComponent<CTransform>();
				if (overlap.x > 0 && overlap.y > 0)
				{
					std::cout << "Overlapping!" << std::endl;
					std::cout << "Overlap X: " << overlap.x << ". Overlap Y: " << overlap.y << std::endl;

					Vec2 prevOverlap = m_game->physics().GetPreviousOverlap(m_player, e);

					if (prevOverlap.x > 0)
					{
						//Overlaps vertically
						if (transform.prevPos.y < transform.pos.y)
						{
							//From the top
							transform.pos.y -= overlap.y;
							playerTouchingGround = true;
						}
						else {
							//From the bottom
							transform.pos.y += overlap.y;
							if (e->getComponent<CAnimation>().animation.getName() == "Brick")
							{
								e->destroy();
							}
						}
						transform.velocity.y = 0;
					}
					else if (prevOverlap.y > 0)
					{
						//Overlaps horizontally
						if (transform.prevPos.x < transform.pos.x)
						{
							//From the left
							transform.pos.x -= overlap.x;
						}
						else {
							//From the right
							transform.pos.x += overlap.x;
						}
					}
					else
					{
						//Overlaps diagonally, correct horizontally
						if (transform.prevPos.x < transform.pos.x)
						{
							//From the left
							transform.pos.x -= overlap.x;
						}
						else {
							//From the right
							transform.pos.x += overlap.x;
						}
					}
				}
				else if (!playerTouchingGround)
				{
					Vec2 tempPos = transform.pos;
					transform.pos.y += 0.2;
					Vec2 testGround = m_game->physics().GetOverlap(m_player, e);
					playerTouchingGround = (testGround.x > 0 && testGround.y > 0);
					transform.pos = tempPos;
				}
			}
		}
	}
	if (playerTouchingGround)
	{
		m_player->getComponent<CInput>().canJump = true;
	}
	m_player->getComponent<CState>().isTouchingGround = playerTouchingGround;

	//TODO: Implement bullet/tile collisions
}

void Scene_Play::sDoAction(const Action& action)
{
	if (action.type() == "START")
	{
		if (action.name() == "TOGGLE_TEXTURE") { m_drawTextures = !m_drawTextures; }
		else if (action.name() == "TOGGLE_COLLISION") { m_drawCollision = !m_drawCollision; }
		else if (action.name() == "TOGGLE_GRID") { m_drawGrid = !m_drawGrid; }
		else if (action.name() == "PAUSE") { setPaused(!m_paused); }
		else if (action.name() == "QUIT") { onEnd(); }
		else if (action.name() == "UP") { m_player->getComponent<CInput>().up = true; }
		else if (action.name() == "LEFT") { m_player->getComponent<CInput>().left = true; }
		else if (action.name() == "RIGHT") { m_player->getComponent<CInput>().right = true; }
		else if (action.name() == "DOWN") { m_player->getComponent<CInput>().down = true; }
		else if (action.name() == "STOP") { m_player->getComponent<CTransform>().velocity = { 0,0 }; }
		else if (action.name() == "RESET") { 
			m_player->getComponent<CTransform>().pos = m_player->getComponent<CTransform>().initialPos; 
		}
	}
	else if (action.type() == "END")
	{
		if (action.name() == "UP") { m_player->getComponent<CInput>().up = false; m_player->getComponent<CInput>().canJump = false; }
		else if (action.name() == "LEFT") { m_player->getComponent<CInput>().left = false; }
		else if (action.name() == "RIGHT") { m_player->getComponent<CInput>().right = false; }
		else if (action.name() == "DOWN") { m_player->getComponent<CInput>().down = false; }
	}
}

void Scene_Play::sAnimation()
{
	//TODO: Set animation based on CState component
	//TODO: For each entity with an animation call ent->getComponent<CAnimation>().animation.update();
	//If animation doesn't repeat and has finished, destroy entity
	auto& state = m_player->getComponent<CState>();
	auto& anim = m_player->getComponent<CAnimation>();
	if (state.state == "stand" && anim.animation.getName() != "Stand")
		m_player->addComponent<CAnimation>(m_game->assets().getAnimation("Stand"), true);

	if (state.state == "running" && anim.animation.getName() != "Run")
		m_player->addComponent<CAnimation>(m_game->assets().getAnimation("Run"), true);

	if (state.state == "falling" && anim.animation.getName() != "Air")
		m_player->addComponent<CAnimation>(m_game->assets().getAnimation("Air"), true);


	for (auto e : m_entityManager.getEntities())
	{
		if (e->hasComponent<CAnimation>())
		{
			auto& anim = e->getComponent<CAnimation>();
			anim.animation.update();
			if (anim.animation.hasEnded() && !anim.repeat)
				e->destroy();
		}
	}
}

void Scene_Play::sLifespan()
{

}

void Scene_Play::onEnd()
{
	//TODO: When scene ends change back to menu scene
	// Use m_game->changeScene(params);
}

void Scene_Play::sRender()
{
	if (!m_paused) { m_game->window().clear(sf::Color(100, 100, 255)); }
	else { m_game->window().clear(sf::Color(50, 50, 150)); }

	auto& pPos = m_player->getComponent<CTransform>().pos;
	float windowCenterX = std::max(m_game->window().getSize().x / 2.0f, pPos.x);
	sf::View view = m_game->window().getView();
	view.setCenter(windowCenterX, m_game->window().getSize().y - view.getCenter().y);
	m_game->window().setView(view);

	uint8_t alpha = m_paused ? 127 : 255;

	for (auto e : m_entityManager.getEntities())
	{
		if (m_drawTextures)
		{
			auto& eTransform = e->getComponent<CTransform>();

			if (e->hasComponent<CAnimation>())
			{
				auto& anim = e->getComponent<CAnimation>().animation;
				anim.getSprite().setRotation(eTransform.angle);
				anim.getSprite().setPosition(eTransform.pos.x, eTransform.pos.y);
				anim.getSprite().setScale(eTransform.scale.x, eTransform.scale.y);
				anim.getSprite().setColor(sf::Color(255, 255, 255, alpha));
				m_game->window().draw(anim.getSprite());
			}
		}
		if (m_drawCollision)
		{
			if (e->hasComponent<CBoundingBox>())
			{
				auto& box = e->getComponent<CBoundingBox>();
				auto& transform = e->getComponent<CTransform>();
				sf::RectangleShape rect;
				rect.setSize(sf::Vector2f(box.size.x - 1, box.size.y - 1));
				rect.setOrigin(sf::Vector2f(box.halfSize.x, box.halfSize.y));
				rect.setPosition(transform.pos.x, transform.pos.y);
				rect.setFillColor(sf::Color(0, 0, 0, 0));
				rect.setOutlineColor(sf::Color(255, 255, 255, alpha));
				rect.setOutlineThickness(1);
				m_game->window().draw(rect);
			}
		}
	}

	if (m_drawGrid)
	{
		float leftX = m_game->window().getView().getCenter().x - width() / 2;
		float rightX = leftX + width() + m_gridSize.x;
		float nextGridX = leftX - ((int)leftX % (int)m_gridSize.x);

		for (float x = nextGridX; x < rightX; x += m_gridSize.x)
		{
			drawLine(Vec2(x, 0), Vec2(x, height()), alpha);
		}

		for (float y = 0; y < height(); y += m_gridSize.y)
		{
			drawLine(Vec2(leftX, height() - y), Vec2(rightX, height() - y), alpha);
			for (float x = nextGridX; x < rightX; x += m_gridSize.x)
			{
				std::string xCell = std::to_string((int)x / (int)m_gridSize.x);
				std::string yCell = std::to_string((int)y / (int)m_gridSize.y);
				m_gridText.setFont(m_game->assets().getFont("Firacode"));
				m_gridText.setString("(" + xCell + "," + yCell + ")");
				m_gridText.setPosition(x + 3, height() - y - m_gridSize.y + 2);
				m_gridText.setFillColor(sf::Color(255, 255, 255, alpha));
				m_game->window().draw(m_gridText);
			}
		}
	}

	if (m_paused) {
		sf::Text pauseText;
		pauseText.setString("PAUSED");
		pauseText.setFont(m_game->assets().getFont("Firacode"));
		pauseText.setCharacterSize(72);
		sf::FloatRect textRect = pauseText.getLocalBounds();
		pauseText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
		pauseText.setPosition(m_game->window().getView().getCenter());
		m_game->window().draw(pauseText);
	}
}