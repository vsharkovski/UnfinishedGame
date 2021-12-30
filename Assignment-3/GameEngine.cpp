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
	m_assets.loadFromFile(path);
	
	m_window.create(sf::VideoMode(1280, 768), "Definitely Not Mario");
	m_window.setFramerateLimit(60);

	changeScene("MENU", std::make_shared<Scene_Menu>(this));
}

std::shared_ptr<Scene> GameEngine::currentScene()
{
	return m_sceneMap[currentScene];
}

bool GameEngine::isRunning()
{
	return m_running & m_window.isOpen();
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
		//if (event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased)
		//{
		//	if (currentScene->getActionMap().find(event.key.code) == currentScene->getActionMap().end())
		//		continue;
		//	const std::string actionType = (event.type == sf::Event::KeyPressed) ? "START" : "END";
		//	currentScene()->doAction(Action(currentScene()->getActionMap().at(event.key.code), actionType));
		//}
	}
}