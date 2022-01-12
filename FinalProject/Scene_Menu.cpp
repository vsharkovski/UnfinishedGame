#include "Scene_Menu.h"
#include "Scene_Level.h"
#include "Scene_Editor.h"

Scene_Menu::Scene_Menu(GameEngine* gameEngine)
	: Scene(gameEngine)
{
	init();
}

void Scene_Menu::init()
{
	registerAction(sf::Keyboard::Escape, "BACK");
	registerAction(sf::Keyboard::Backspace, "BACK");
	registerAction(sf::Keyboard::Space, "CONFIRM");
	registerAction(sf::Keyboard::Enter, "CONFIRM");
	registerAction(sf::Keyboard::D, "CONFIRM");
	registerAction(sf::Keyboard::W, "UP");
	registerAction(sf::Keyboard::Up, "UP");
	registerAction(sf::Keyboard::S, "DOWN");
	registerAction(sf::Keyboard::Down, "DOWN");

	m_menuText.setFont(m_game->assets().getFont("Consolas"));

	m_title = "Game";

	m_currentScreen = "MENU";
	m_selectedIndex = 0;

	// menu
	m_menuItems = { "PLAY", "EDITOR", "OPTIONS", "EXIT" };

	// editor
	

	// options

}

void Scene_Menu::update()
{
	m_currentFrame++;
}

void Scene_Menu::onEnd()
{
	m_game->quit();
}

void Scene_Menu::sDoAction(const Action& action)
{
	if (action.type() == "START")
	{
		if (action.name() == "BACK")
		{
			if (m_currentScreen == "MENU") { onEnd(); }
			else { m_currentScreen = "MENU"; m_selectedIndex = 0; }
		}
		else if (action.name() == "CONFIRM")
		{
			if (m_currentScreen == "MENU")
			{
				if (m_menuItems[m_selectedIndex] == "PLAY") { m_game->changeScene("LEVEL", std::make_shared<Scene_Level>(m_game)); }
				else if (m_menuItems[m_selectedIndex] == "EDITOR") { m_game->changeScene("EDITOR", std::make_shared<Scene_Editor>(m_game)); }
				else if (m_menuItems[m_selectedIndex] == "EXIT") { onEnd(); }
				else if (m_menuItems[m_selectedIndex] == "OPTIONS") {}
			}
			else if (m_currentScreen == "EDITOR")
			{

			}
			else if (m_currentScreen == "OPTIONS")
			{

			}
		}
		else if (action.name() == "DOWN")
		{
			if (m_currentScreen == "MENU") { m_selectedIndex = (m_selectedIndex + 1) % m_menuItems.size(); }
			else if (m_currentScreen == "EDITOR") { }
			else if (m_currentScreen == "OPTIONS") { }
		}
		else if (action.name() == "UP")
		{
			if (m_currentScreen == "MENU") { m_selectedIndex = (m_selectedIndex - 1 + m_menuItems.size()) % m_menuItems.size(); }
			else if (m_currentScreen == "EDITOR") {}
			else if (m_currentScreen == "OPTIONS") { }
		}
	}
	else if (action.type() == "END")
	{

	}
	else if (action.name() == "MOUSE_MOVE")
	{
		
	}
}

void Scene_Menu::sRender()
{
	// reset view to normal
	sf::View view = m_game->window().getView();
	view.setCenter(static_cast<float>(width()) / 2.0f, static_cast<float>(height()) / 2.0f);
	m_game->window().setView(view);

	// draw text
	m_game->window().clear(sf::Color(0, 0, 0));

	m_menuText.setString(m_title);
	m_menuText.setCharacterSize(80);
	m_menuText.setFillColor(sf::Color(255, 255, 255));
	m_menuText.setPosition(150.0f, 100.0f);
	m_game->window().draw(m_menuText);

	m_menuText.setCharacterSize(40);

	if (m_currentScreen == "MENU")
	{
		for (size_t i = 0; i < m_menuItems.size(); i++)
		{
			m_menuText.setString(m_menuItems[i]);
			if (i == m_selectedIndex)
				m_menuText.setFillColor(sf::Color(0, 50, 250));
			else
				m_menuText.setFillColor(sf::Color(255, 255, 255));

			m_menuText.setPosition(200.0f, 300.0f + 100.0f * static_cast<float>(i));
			m_game->window().draw(m_menuText);
		}
	}
	else if (m_currentScreen == "EDITOR")
	{

	}
	else if (m_currentScreen == "OPTIONS")
	{

	}
}