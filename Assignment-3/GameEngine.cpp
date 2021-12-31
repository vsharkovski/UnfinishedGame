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
	
	const int windowWidth = 1280; // 1280
	const int windowHeight = 768; // 768

	m_window.create(
		sf::VideoMode(windowWidth, windowHeight),
		"Definitely Not Mario",
		sf::Style::None);
		//sf::Style::Titlebar | sf::Style::Close);
	m_window.setFramerateLimit(60);

	changeScene("MENU", std::make_shared<Scene_Menu>(this));
	//changeScene("LEVEL", std::make_shared<Scene_Play>(this, "config/level1.txt"));
}

void GameEngine::changeScene(const std::string& sceneName, std::shared_ptr<Scene> scene, bool endCurrentScene)
{
	if (endCurrentScene && m_sceneMap.find(m_currentScene) != m_sceneMap.end())
	{
		m_sceneMap.erase(m_currentScene);
	}
	m_currentScene = sceneName;
	if (scene)
	{
		m_sceneMap[m_currentScene] = scene;
	}
}

std::shared_ptr<Scene> GameEngine::currentScene()
{
	return m_sceneMap[m_currentScene];
}

void GameEngine::update()
{
	currentScene()->update();
}

void GameEngine::run()
{
	while (isRunning())
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
			if (currentScene()->getActionMap().find(event.key.code) == currentScene()->getActionMap().end())
			{
				continue;
			}
			const std::string actionType = (event.type == sf::Event::KeyPressed) ? "START" : "END";
			currentScene()->doAction(Action(currentScene()->getActionMap().at(event.key.code), actionType));
		}
	}
}

