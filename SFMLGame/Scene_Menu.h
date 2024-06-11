#pragma once

#include "Common.h"
#include "Scene.h"
#include <map>
#include <memory>
#include <deque>

class Action;
class GameEngine;

class Scene_Menu : public Scene
{
protected:
	std::string m_title;
	std::vector<std::string> m_menuStrings;
	std::vector<std::string> m_levelPaths;
	sf::Text m_menuText;
	sf::RectangleShape m_rect;
	size_t m_selectedMenuIndex = 0;

	void init();
	void onEnd();
	void sDoAction(const Action& action);
	void sRender();

public:
	Scene_Menu(GameEngine* gameEngine = nullptr);
	void update();
};

