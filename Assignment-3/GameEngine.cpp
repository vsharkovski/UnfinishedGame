#include "GameEngine.h"
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

	//changeScene("MENU", std::make_shared<Scene_Menu>(this));
	changeScene("LEVEL", std::make_shared<Scene_Play>(this, "config/level1.txt"));
}

void GameEngine::changeScene(const std::string& sceneName, std::shared_ptr<Scene> scene, bool endCurrentScene)
{
	if (endCurrentScene && m_sceneMap.find(m_currentScene) != m_sceneMap.end())
	{
		m_sceneMap.erase(m_currentScene);
	}
	m_currentScene = sceneName;
	m_sceneMap[m_currentScene] = scene;
}

std::shared_ptr<Scene> GameEngine::currentScene()
{
	//std::cout << "getting currentScene (" << m_currentScene << ")" << std::endl;
	return m_sceneMap[m_currentScene];
}

void GameEngine::update()
{
	currentScene()->update();
}

void GameEngine::run()
{
	while (m_running)
	{
		update();
		sUserInput();
	}
}

void GameEngine::quit()
{
	m_running = false;
}

const bool GameEngine::isRunning()
{
	return m_running & m_window.isOpen();
}

sf::RenderWindow& GameEngine::window()
{
	return m_window;
}

const Assets& GameEngine::assets() const
{
	return m_assets;
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
		else if (event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased)
		{
			std::cout << "keyCode=" << event.key.code << std::endl;
			/*std::cout << "actionMap:";
			for (auto& kvp : currentScene()->getActionMap())
			{
				std::cout << "(" << kvp.first << "," << kvp.second << "),";
			}*/
			std::cout << std::endl;
			if (currentScene()->getActionMap().find(event.key.code) == currentScene()->getActionMap().end())
			{
				continue;
			}
			const std::string actionType = (event.type == sf::Event::KeyPressed) ? "START" : "END";
			std::cout << "actionType=" << actionType << std::endl;
			currentScene()->doAction(Action(currentScene()->getActionMap().at(event.key.code), actionType));
		}
	}
}

