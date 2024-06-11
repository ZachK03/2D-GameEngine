#include "Scene_Menu.h"
#include "Scene_Play.h"
#include "Common.h"
#include "Assets.h"
#include "GameEngine.h"
#include "Components.h"

Scene_Menu::Scene_Menu(GameEngine* gameEngine)
	: Scene(gameEngine)
{
	init();
	std::cout << "Scene Menu created" << std::endl;
}

void Scene_Menu::init()
{
	registerAction(sf::Keyboard::W, "UP");
	registerAction(sf::Keyboard::S, "DOWN");
	registerAction(sf::Keyboard::Enter, "SELECT");

	m_title = "Mega Mario";
	m_menuStrings.push_back("Level 1");
	m_menuStrings.push_back("Level 2");
	m_menuStrings.push_back("Level 3");

	m_levelPaths.push_back("levels/level1.txt");
	m_levelPaths.push_back("levels/level2.txt");
	m_levelPaths.push_back("levels/level3.txt");

	//m_menuText.setFont(m_game->assets().getFont(""));

	m_menuText.setFont(m_game->assets().getFont("Firacode"));
	m_menuText.setPosition(5.0f, 0.0f);
	m_menuText.setString(m_title);
	m_menuText.setFillColor(sf::Color::Red);
	m_menuText.setCharacterSize(48);
}

void Scene_Menu::update()
{
	sRender();
}

void Scene_Menu::onEnd()
{

}

void Scene_Menu::sDoAction(const Action& action)
{
	if (action.type() == "START")
	{
		if (action.name() == "UP") {
			if (m_selectedMenuIndex == 0)
				m_selectedMenuIndex = m_menuStrings.size();
			m_selectedMenuIndex--;
		}
		else if (action.name() == "DOWN") {
			m_selectedMenuIndex++;
			if (m_selectedMenuIndex == m_menuStrings.size())
				m_selectedMenuIndex = 0;
		}
		else if (action.name() == "SELECT") {
			m_game->changeScene("GAME", std::make_shared<Scene_Play>(m_game, m_levelPaths.at(m_selectedMenuIndex)));
		}
	}
	else
	{

	}
}

void Scene_Menu::sRender()
{
	int temp = 0;
	for (auto& str : m_menuStrings)
	{
		sf::Text levelOption;
		levelOption.setFont(m_game->assets().getFont("Firacode"));
		levelOption.setString(str);
		levelOption.setFillColor(sf::Color::White);
		levelOption.setCharacterSize(36);
		sf::Vector2f tempPos(5.0f, 48.0f + (temp * 40.0f));
		levelOption.setPosition(tempPos);
		if (temp == m_selectedMenuIndex)
			levelOption.setStyle(sf::Text::Bold);
		m_game->window().draw(levelOption);
		temp++;
	}
	m_game->window().draw(m_menuText);
}