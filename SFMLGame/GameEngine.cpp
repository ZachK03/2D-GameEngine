#include "GameEngine.h"
#include "Assets.h"
#include "Scene_Play.h"
#include "Scene_Menu.h"

GameEngine::GameEngine(const std::string& path)
{
	init(path);
}

void GameEngine::init(const std::string& path)
{
	m_assets.loadFromFile("assets.txt");

	m_window.create(sf::VideoMode(1280, 768), "Definitely Not Mario");
	m_window.setFramerateLimit(60);

	changeScene("MENU", std::make_shared<Scene_Menu>(this));
}

void GameEngine::update()
{
	sUserInput();
	m_window.clear(sf::Color::Black);
	currentScene()->update();

	m_window.display();
}

void GameEngine::getLevels()
{
	std::string path = "/levels";
	for (const auto& entry : std::filesystem::directory_iterator(path))
	{
		std::ifstream file(entry.path());
		std::string str;
		std::string name;
		while (file.good())
		{
			file >> str;
			if (str == "Name")
			{
				file >> name;
			}
		}
	}
}

void GameEngine::sUserInput()
{
	sf::Event event;
	while (m_window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
		{
			quit();
		}

		if (event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased)
		{
			if (currentScene()->getActionMap().find(event.key.code) == currentScene()->getActionMap().end())
				continue;

			const std::string actionType = (event.type == sf::Event::KeyPressed) ? "START" : "END";
			Action action(currentScene()->getActionMap().at(event.key.code), actionType);
			currentScene()->sDoAction(action);
		}
	}
}

std::shared_ptr<Scene> GameEngine::currentScene()
{
	return m_sceneMap[m_currentScene];
}

void GameEngine::changeScene(const std::string& sceneName, std::shared_ptr<Scene> scene, bool endCurrentScene)
{
	if (!scene)
	{
		std::cerr << "No scene provided for changing scene." << std::endl;
		return;
	}
	m_sceneMap[sceneName] = scene;
	if (endCurrentScene)
	{
		scene->endScene();
		m_sceneMap.erase(m_sceneMap.find(m_currentScene));
	}
	m_currentScene = sceneName;
	std::cout << "Scene changed" << std::endl;
}

void GameEngine::quit()
{
	m_running = false;
}

void GameEngine::run()
{
	while (isRunning())
	{
		update();
	}

	m_window.close();
}

sf::RenderWindow& GameEngine::window()
{
	return m_window;
}

const Assets& GameEngine::assets() const
{
	return m_assets;
}

bool GameEngine::isRunning()
{
	return m_running && m_window.isOpen();
}

const Physics& GameEngine::physics() const
{
	return m_physics;
}