#include "Scene_Menu.h"
#include "Scene_Zelda.h"

Scene_Menu::Scene_Menu(GameEngine* gameEngine)
	: Scene(gameEngine)
{
	init();
}

void Scene_Menu::init()
{
	registerAction(sf::Keyboard::Escape, "QUIT");
	registerAction(sf::Keyboard::W, "UP");
	registerAction(sf::Keyboard::S, "DOWN");
	registerAction(sf::Keyboard::D, "PLAY");
	
	m_title = "Not Zelda";

	m_menuStrings.push_back("Level  1");
	m_menuStrings.push_back("Level  2");
	m_menuStrings.push_back("Level  3");

	m_levelPaths.push_back("level.txt");
	m_levelPaths.push_back("level.txt");
	m_levelPaths.push_back("level.txt");

	m_selectedItem = 0;

	m_menuText.setFont(m_game->assets().getFont("Consolas"));

	m_game->playSound("MusicTitle");
}

void Scene_Menu::update()
{
	m_currentFrame++;
}

void Scene_Menu::sDoAction(const Action& action)
{
	if (action.type() == "START")
	{
		if (action.name() == "UP")
		{
			m_selectedItem = m_selectedItem == 0 ? m_menuStrings.size() - 1 : m_selectedItem - 1;
		}
		else if (action.name() == "DOWN")
		{
			m_selectedItem = m_selectedItem + 1 == m_menuStrings.size() ? 0 : m_selectedItem + 1;
		}
		else if (action.name() == "PLAY")
		{
			m_game->stopSound("MusicTitle");
			m_game->changeScene("LEVEL", std::make_shared<Scene_Zelda>(m_game, m_levelPaths[m_selectedItem]));
		}
		else if (action.name() == "QUIT")
		{
			onEnd();
		}
	}
}

void Scene_Menu::onEnd()
{
	m_game->quit();
}

void Scene_Menu::sRender()
{
	m_game->window().clear(sf::Color(0, 0, 0));

	// reset view to normal
	sf::View view = m_game->window().getView();
	view.setCenter(static_cast<float>(width()) / 2.0f, static_cast<float>(height()) / 2.0f);
	m_game->window().setView(view);

	// strings
	m_menuText.setString(m_title);
	m_menuText.setCharacterSize(80);
	m_menuText.setFillColor(sf::Color(255, 255, 255));
	m_menuText.setPosition(150.0f, 100.0f);
	m_game->window().draw(m_menuText);

	m_menuText.setCharacterSize(40);
	for (size_t i = 0; i < m_menuStrings.size(); i++)
	{
		m_menuText.setString(m_menuStrings[i]);
		if (i == m_selectedItem)
			m_menuText.setFillColor(sf::Color(0, 50, 250));
		else
			m_menuText.setFillColor(sf::Color(255, 255, 255));

		m_menuText.setPosition(200.0f, 300.0f + 100.0f * static_cast<float>(i));
		m_game->window().draw(m_menuText);
	}

	m_menuText.setString("W: prev level    S: next level    D: PLAY   ESC: exit");
	m_menuText.setCharacterSize(30);
	m_menuText.setFillColor(sf::Color(255, 255, 255));
	m_menuText.setPosition(150.0f, static_cast<float>(height()) - 100.0f);
	m_game->window().draw(m_menuText);

	m_menuText.setString("In-game controls:\n    W: move up\n    A: move left\n    S: move down\n    D: move right\n    SPACE: attack");
	m_menuText.setPosition(static_cast<float>(width()) - 500.0f, 300.0f);
	m_game->window().draw(m_menuText);
}