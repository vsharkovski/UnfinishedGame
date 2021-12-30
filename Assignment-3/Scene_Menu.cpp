#include "Scene_Menu.h"
#include "Scene_Play.h"
#include "Common.h"
#include "Assets.h"
#include "GameEngine.h"
#include "Components.h"
#include "Action.h"

Scene_Menu::Scene_Menu(GameEngine* gameEngine)
	: Scene(gameEngine)
{
	init();
}

void Scene_Menu::init()
{
	registerAction(sf::Keyboard::W, "UP");
	registerAction(sf::Keyboard::S, "DOWN");
	registerAction(sf::Keyboard::D, "PLAY");
	registerAction(sf::Keyboard::Escape, "QUIT");
	
	m_title = "Mega Mario";
	m_menuStrings.push_back("Level  1");
	//m_menuStrings.push_back("Level  2");
	//m_menuStrings.push_back("Level  3");

	m_levelPaths.push_back("level1.txt");
	//m_levelPaths.push_back("level1.txt");
	//m_levelPaths.push_back("level1.txt");

	m_menuText.setFont(m_game->assets().getFont("Arial"));
}