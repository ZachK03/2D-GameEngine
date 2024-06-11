#pragma once

#include <memory>
#include <filesystem>
#include <iostream>
#include <fstream>
#include "Physics.h"

#include "Scene.h"

typedef std::map<std::string, std::shared_ptr<Scene>> SceneMap;
class GameEngine
{
protected:
	sf::RenderWindow m_window;
	Assets m_assets;
	std::string m_currentScene;
	SceneMap m_sceneMap;
	size_t m_simulationSpeed = 1;
	Physics m_physics;
	bool m_running = true;

	void init(const std::string& path);
	void update();
	void getLevels();

	void sUserInput();

	std::shared_ptr<Scene> currentScene();

public:
	GameEngine(const std::string& path);

	void changeScene(const std::string& sceneName, std::shared_ptr<Scene> scene, bool endCurrentScene = false);

	void quit();
	void run();
	
	sf::RenderWindow& window();
	const Assets& assets() const;
	bool isRunning();
	const Physics& physics() const;
};