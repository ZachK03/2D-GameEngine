#include "Game.h"

Game::Game(const std::string& config) {
	init(config);
}

Game::Game(const std::string& config, bool debug) {
	m_debug = debug;
	init(config);
}

void Game::init(const std::string& path) {
	srand((unsigned int)time(NULL));
	std::ifstream fin(path);

	if (!fin.is_open()) {
		std::cout << "Failed to open file\n";
	}

	int wWidth = 1280;
	int wHeight = 720;
	int wFrameLimit = 60;
	int wFullScreen = 0;
	std::string mode;
	if (m_debug) {
		std::cout << "--CONFIG LOADING--\n";
	}
	while (fin >> mode) {
		if(m_debug) 
			std::cout << mode << '\n';
		if (mode == "Window") {
			fin >> wWidth >> wHeight >> wFrameLimit >> wFullScreen;
		}
		if (mode == "Font") {
			std::string font;
			int size, r, g, b;
			fin >> font >> size >> r >> g >> b;
			if (!m_font.loadFromFile(font)) {
				std::cerr << "Failed to load font.\n";
				return;
			}
			m_text.setFont(m_font);
			m_text.setCharacterSize(size);
			m_text.setFillColor(sf::Color(r, g, b));
		}
		if (mode == "Player") {
			fin >> m_playerConfig.SR >> m_playerConfig.CR >> m_playerConfig.S >> m_playerConfig.FR >> m_playerConfig.FG >> m_playerConfig.FB >> m_playerConfig.OR >> m_playerConfig.OG >> m_playerConfig.OB >> m_playerConfig.OT >> m_playerConfig.V;
		}
		if (mode == "Enemy") {
			fin >> m_enemyConfig.SR >> m_enemyConfig.CR >> m_enemyConfig.SMIN >> m_enemyConfig.SMAX >> m_enemyConfig.OR >> m_enemyConfig.OG >> m_enemyConfig.OB >> m_enemyConfig.OT >> m_enemyConfig.VMIN >> m_enemyConfig.VMAX >> m_enemyConfig.L >> m_enemyConfig.SI;
		}
		if (mode == "Bullet") {
			fin >> m_bulletConfig.SR >> m_bulletConfig.CR >> m_bulletConfig.S >> m_bulletConfig.FR >> m_bulletConfig.FG >> m_bulletConfig.FB >> m_bulletConfig.OR >> m_bulletConfig.OG >> m_bulletConfig.OB >> m_bulletConfig.OT >> m_bulletConfig.V >> m_bulletConfig.L;
		}
	}
	if (m_debug) {
		std::cout << "--END CONFIG LOADING--\n\n";
	}

	auto style = sf::Style::Default;
	if (wFullScreen != 0) {
		style = sf::Style::Fullscreen;
	}

	m_window.create(sf::VideoMode(wWidth, 720), "Assignment 2", style);
	m_window.setFramerateLimit(wFrameLimit);

	spawnPlayer();
}

void Game::run() {
	while (m_running) {
		m_entities.update();

		if (!m_isPaused) {
			sLifespan();
			sEnemySpawner();
			sMovement();
			sCollision();
			m_currentFrame++;
		}
		sUserInput();
		sRender();
	}
}

void Game::setPaused(bool paused) {
	m_isPaused = paused;
}

void Game::spawnPlayer() {
	auto entity = m_entities.addEntity("player");
	float mx = m_window.getSize().x / 2.0f;
	float my = m_window.getSize().y / 2.0f;

	entity->addComponent<CTransform>(Vec2(mx, my), Vec2(0.0f, 0.0f), Vec2{ 1.0,1.0 }, 0.0f);
	entity->addComponent<CShape>(m_playerConfig.SR, m_playerConfig.V, sf::Color(m_playerConfig.FR, m_playerConfig.FG, m_playerConfig.FB), sf::Color(m_playerConfig.OR, m_playerConfig.OG, m_playerConfig.OB), m_playerConfig.OT);
	entity->addComponent<CInput>();
	entity->addComponent<CCollision>(m_playerConfig.CR);

	m_player = entity;
}

void Game::spawnEnemy() {
	auto entity = m_entities.addEntity("enemy");

	//Get new position until position is far from player position
	Vec2 pos;
	do {
		float ex = rand() % m_window.getSize().x;
		float ey = rand() % m_window.getSize().y;
		pos.x = ex;
		pos.y = ey;
	} while (pos.dist(m_player->getComponent<CTransform>().pos) < (m_playerConfig.CR * 2));

	int verticies = (rand() % (m_enemyConfig.VMAX - m_enemyConfig.VMIN)) + m_enemyConfig.VMIN;
	int r = rand() % 255;
	int g = rand() % 255;
	int b = rand() % 255;

	float speed = (rand() % (int)(m_enemyConfig.SMAX - m_enemyConfig.SMIN)) + m_enemyConfig.SMIN;
	float dir = rand() % 360;
	Vec2 relPos = pos.getRelPos(1.0f, dir);
	relPos += pos;
	Vec2 normalizedVectorSpeed = pos.normalize(relPos);
	normalizedVectorSpeed *= speed;

	if (m_debug) {
		std::cout << "--ENEMY--\n";
		std::cout << "Speed: " << speed << "\nRelPos: (" << relPos.x << ", " << relPos.y << ")\nSpeed Vector: (" << normalizedVectorSpeed.x << ", " << normalizedVectorSpeed.y << ")\n";
		std::cout << "--END ENEMY--\n\n";
	}

	entity->addComponent<CTransform>(pos, normalizedVectorSpeed, Vec2{ 1.0,1.0 }, 0.0f);
	entity->addComponent<CShape>(m_enemyConfig.SR, verticies, sf::Color(r, g, b), sf::Color(m_enemyConfig.OR, m_enemyConfig.OG, m_enemyConfig.OB), m_enemyConfig.OT);
	entity->addComponent<CCollision>(m_enemyConfig.CR);
	entity->addComponent<CScore>(verticies * 100);

	m_lastEnemySpawnTime = m_currentFrame;
}

void Game::spawnSmallEnemies(std::shared_ptr<Entity> entity) {
	//Spawns small enemies at the location of the input enemy e
	// when a smaller enemy is created we need to read values from original enemy
	// - spawn a number of small enemies equal to the verticies
	// - set each small enemy to the same color as the original, half the size
	// - small enemies are worth double points of the original enemy

	float radius = entity->getComponent<CShape>().circle.getRadius();
	auto fillColor = entity->getComponent<CShape>().circle.getFillColor();
	auto outlineColor = entity->getComponent<CShape>().circle.getOutlineColor();
	float outlineThickness = entity->getComponent<CShape>().circle.getOutlineThickness();
	Vec2 pos(entity->getComponent<CShape>().circle.getPosition().x, entity->getComponent<CShape>().circle.getPosition().y);
	float speed = entity->getComponent<CTransform>().velocity.length();
	float rotation = entity->getComponent<CShape>().circle.getRotation();

	int verticies = entity->getComponent<CShape>().circle.getPointCount();
	if (m_debug) {
		std::cout << "--SMALL ENEMY--\n";
		std::cout << "Verticies: " << verticies << '\n';
	}
	int interval = 360 / verticies;
	for (int i = 0; i < verticies; i++) {
		auto smallEnemy = m_entities.addEntity("smallenemy");
		float dir = (float)(i * interval) + rotation;
	
		Vec2 relPos = pos.getRelPos(1.0f, dir);
		if (m_debug) {
			std::cout << "Dir: " << dir << "\nRelPos: " << relPos;
		}
		relPos += pos;
		if (m_debug) {
			std::cout << "\nPos: " << pos << "\nRelPos Updated: " << relPos;
		}
		Vec2 normalizedVectorSpeed = pos.normalize(relPos);
		normalizedVectorSpeed *= speed;
		if (m_debug) {
			std::cout << "\nSpeed Vector: " << normalizedVectorSpeed << "\n";
		}

		smallEnemy->addComponent<CTransform>(pos, normalizedVectorSpeed, Vec2{ 1.0,1.0 }, 0.0f);
		smallEnemy->addComponent<CShape>(radius / 2.0f, verticies, fillColor, outlineColor, outlineThickness);
		smallEnemy->addComponent<CCollision>(m_enemyConfig.CR / 2.0f);
		smallEnemy->addComponent<CLifespan>(m_enemyConfig.L,m_currentFrame);
		smallEnemy->addComponent<CScore>(verticies * 200);
		
	}
	std::cout << "--END SMALL ENEMY--\n\n";

	entity->destroy();
}

void Game::spawnBullet(std::shared_ptr<Entity> entity, const Vec2& target) {
	auto bullet = m_entities.addEntity("bullet");
	float speed = m_bulletConfig.S;
	Vec2 normVector = entity->getComponent<CTransform>().pos.normalize(target);
	normVector *= speed;
	float angle = entity->getComponent<CTransform>().pos.angle(target);

	bullet->addComponent<CTransform>(Vec2(entity->getComponent<CTransform>().pos.x, entity->getComponent<CTransform>().pos.y), normVector, Vec2{ 1.0,1.0 }, angle);
	bullet->addComponent<CShape>(m_bulletConfig.SR, m_bulletConfig.V, sf::Color(m_bulletConfig.FR, m_bulletConfig.FG, m_bulletConfig.FB), sf::Color(m_bulletConfig.OR, m_bulletConfig.OG, m_bulletConfig.OB), m_bulletConfig.OT);
	bullet->addComponent<CLifespan>(m_bulletConfig.L,m_currentFrame);
	bullet->addComponent<CCollision>(m_bulletConfig.CR);
}

void Game::spawnSpecialWeapon(std::shared_ptr<Entity> entity) {
	//TODO: special weapon
}

void Game::sRender() {
	m_window.clear();

	m_text.setPosition(0.0f,0.0f);
	m_text.setString("Score: " + std::to_string(m_score));
	m_window.draw(m_text);

	m_player->getComponent<CShape>().circle.setPosition(m_player->getComponent<CTransform>().pos.x, m_player->getComponent<CTransform>().pos.y);

	m_player->getComponent<CTransform>().angle += 1.0f;
	m_player->getComponent<CShape>().circle.setRotation(m_player->getComponent<CTransform>().angle);

	m_window.draw(m_player->getComponent<CShape>().circle);

	for (auto& e : m_entities.getEntities()) {
		e->getComponent<CShape>().circle.setPosition(e->getComponent<CTransform>().pos.x, e->getComponent<CTransform>().pos.y);
		e->getComponent<CTransform>().angle += 1.0f;
		e->getComponent<CShape>().circle.setRotation(e->getComponent<CTransform>().angle);

		m_window.draw(e->getComponent<CShape>().circle);
	}

	m_window.display();
}

void Game::sUserInput() {
	sf::Event event;
	while (m_window.pollEvent(event)) {
		if (event.type == sf::Event::Closed) {
			m_running = false;
		}
		if (event.type == sf::Event::KeyPressed) {
			switch (event.key.code) {
			case sf::Keyboard::W:
				m_player->getComponent<CInput>().up = true;
				break;
			case sf::Keyboard::A:
				m_player->getComponent<CInput>().left = true;
				break;
			case sf::Keyboard::S:
				m_player->getComponent<CInput>().down = true;
				break;
			case sf::Keyboard::D:
				m_player->getComponent<CInput>().right = true;
				break;
			case sf::Keyboard::P:
				setPaused(!m_isPaused);
				break;
			case sf::Keyboard::Escape:
				m_running = false;
				break;
			default: break;
			}
		}
		if (event.type == sf::Event::KeyReleased) {
			switch (event.key.code) {
			case sf::Keyboard::W:
				m_player->getComponent<CInput>().up = false;
				break;
			case sf::Keyboard::A:
				m_player->getComponent<CInput>().left = false;
				break;
			case sf::Keyboard::S:
				m_player->getComponent<CInput>().down = false;
				break;
			case sf::Keyboard::D:
				m_player->getComponent<CInput>().right = false;
				break;
			default: break;
			}
		}
		if (event.type == sf::Event::MouseButtonPressed && !m_isPaused) {
			if (event.mouseButton.button == sf::Mouse::Left) {
				spawnBullet(m_player, Vec2(event.mouseButton.x, event.mouseButton.y));
				if (m_debug) {
					std::cout << "--MOUSE CLICK--\n";
					std::cout << "Left mouse pressed at (" << event.mouseButton.x << ", " << event.mouseButton.y << ")\n";
					std::cout << "--END MOUSE CLICK--\n\n";
				}
			}
			if (event.mouseButton.button == sf::Mouse::Right) {
				//Special weapon
			}
		}
	}
}

void Game::sMovement() {
	//	- Prevent entities from moving outside of the window
	float windowWidth = m_window.getSize().x;
	float windowHeight = m_window.getSize().y;

	m_player->getComponent<CTransform>().velocity.x = 0;
	m_player->getComponent<CTransform>().velocity.y = 0;
	if (m_player->getComponent<CInput>().up) {
		m_player->getComponent<CTransform>().velocity.y -= m_playerConfig.S;
	}
	if (m_player->getComponent<CInput>().down) {
		m_player->getComponent<CTransform>().velocity.y += m_playerConfig.S;
	}
	if (m_player->getComponent<CInput>().left) {
		m_player->getComponent<CTransform>().velocity.x -= m_playerConfig.S;
	}
	if (m_player->getComponent<CInput>().right) {
		m_player->getComponent<CTransform>().velocity.x += m_playerConfig.S;
	}

	//Update positions of all entities and prevent exiting the window
	for (auto& e : m_entities.getEntities()) {
		e->getComponent<CTransform>().pos.x += e->getComponent<CTransform>().velocity.x;
		if (e->getComponent<CTransform>().pos.x <= e->getComponent<CShape>().circle.getRadius()) {
			e->getComponent<CTransform>().velocity.x *= -1.0f;
			e->getComponent<CTransform>().pos.x = e->getComponent<CShape>().circle.getRadius();
		}
		if (e->getComponent<CTransform>().pos.x >= (windowWidth - e->getComponent<CShape>().circle.getRadius())) {
			e->getComponent<CTransform>().velocity.x *= -1.0f;
			e->getComponent<CTransform>().pos.x = windowWidth - e->getComponent<CShape>().circle.getRadius();
		}
		e->getComponent<CTransform>().pos.y += e->getComponent<CTransform>().velocity.y;
		if (e->getComponent<CTransform>().pos.y <= e->getComponent<CShape>().circle.getRadius()) {
			e->getComponent<CTransform>().velocity.y *= -1.0f;
			e->getComponent<CTransform>().pos.y = e->getComponent<CShape>().circle.getRadius();
		}
		if (e->getComponent<CTransform>().pos.y >= (windowHeight - e->getComponent<CShape>().circle.getRadius())) {
			e->getComponent<CTransform>().velocity.y *= -1.0f;
			e->getComponent<CTransform>().pos.y = windowHeight - e->getComponent<CShape>().circle.getRadius();
		}
	}
}

void Game::sCollision() {
	for (auto& b : m_entities.getEntities("bullet")) {
		for(auto& e : m_entities.getEntities("enemy")) {
			if (isColliding(b, e)) {
				if (m_debug) {
					std::cout << "--COLLISION--\n";
					std::cout << "Collision with bullet and enemy\n";
					std::cout << "--END COLLISION--\n\n";
				}
				m_score += e->getComponent<CScore>().score;
				b->destroy();
				spawnSmallEnemies(e);
			}
		}
		for (auto& e : m_entities.getEntities("smallenemy")) {
			if (isColliding(b, e)) {
				if (m_debug) {
					std::cout << "--COLLISION--\n";
					std::cout << "Collision with bullet and small enemy, double points\n";
					std::cout << "--END COLLISION--\n\n";
				}
				m_score += e->getComponent<CScore>().score;
				b->destroy();
				e->destroy();
			}
		}
	}

	for (auto& e : m_entities.getEntities("enemy")) {
		if (isColliding(m_player, e)) {
			if (m_debug) {
				std::cout << "--COLLISION--\n";
				std::cout << "Collision with enemy and player, reset to middle\n";
				std::cout << "--END COLLISION--\n\n";
			}
			float mx = m_window.getSize().x / 2.0f;
			float my = m_window.getSize().y / 2.0f;
			m_player->getComponent<CTransform>().pos = Vec2(mx, my);
			spawnSmallEnemies(e);
		}
	}
}

bool Game::isColliding(std::shared_ptr<Entity> e1, std::shared_ptr<Entity> e2) const {
	float radiusSum = e1->getComponent<CCollision>().radius + e2->getComponent<CCollision>().radius;
	Vec2 e1Pos(e1->getComponent<CShape>().circle.getPosition().x, e1->getComponent<CShape>().circle.getPosition().y);
	Vec2 e2Pos(e2->getComponent<CShape>().circle.getPosition().x, e2->getComponent<CShape>().circle.getPosition().y);
	float dist = e1Pos.dist(e2Pos);
	if (dist < radiusSum) {
		return true;
	}
	return false;
}

void Game::sLifespan() {
	//For all entities
	//	If entity has no lifespan component, skip it
	//	If entity has > 0 remaining lifespan subtract 1
	//	If has lifespan and is alive scale alpha channel properly
	//	If has lifespan and time is up destroy enemy

	for (auto& e : m_entities.getEntities()) {
		if (!e->hasComponent<CLifespan>()) { continue; };
		auto& lifespan = e->getComponent<CLifespan>();
		int framesRemaining = (lifespan.frameCreated + lifespan.lifespan) - m_currentFrame;
		if (framesRemaining < 0) {
			e->destroy();
			continue;
		}
		float percentAlpha = (float)framesRemaining / (float)e->getComponent<CLifespan>().lifespan;
		e->getComponent<CShape>().circle.setFillColor(sf::Color(e->getComponent<CShape>().circle.getFillColor().r, e->getComponent<CShape>().circle.getFillColor().g, e->getComponent<CShape>().circle.getFillColor().b, 255.0f * percentAlpha));
		e->getComponent<CShape>().circle.setOutlineColor(sf::Color(e->getComponent<CShape>().circle.getOutlineColor().r, e->getComponent<CShape>().circle.getOutlineColor().g, e->getComponent<CShape>().circle.getOutlineColor().b, 255.0f * percentAlpha));
	}
}

void Game::sEnemySpawner() {
	int framesBetweenSpawns = m_enemyConfig.SI;
	if (m_currentFrame - m_lastEnemySpawnTime >= framesBetweenSpawns) {
		spawnEnemy();
	}
}
