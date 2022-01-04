#include "GameEngine.h"
#include "Scene_Menu.h"

GameEngine::GameEngine(const std::string& path)
{
	init(path);
}

void GameEngine::init(const std::string& path)
{
	m_assets.loadFromFile(path);

	const int windowWidth = 1280; // 1280
	const int windowHeight = 720; // 768

	m_window.create(
		sf::VideoMode(windowWidth, windowHeight),
		"Definitely Not Zelda",
		sf::Style::None);
	//sf::Style::Titlebar | sf::Style::Close);
	m_window.setFramerateLimit(60);

	changeScene("MENU", std::make_shared<Scene_Menu>(this));
}

void GameEngine::changeScene(const std::string& sceneName, std::shared_ptr<Scene> scene, bool endCurrentScene)
{
	if (scene)
	{
		m_sceneMap[sceneName] = scene;
	}
	else
	{
		if (m_sceneMap.find(sceneName) == m_sceneMap.end())
		{
			std::cerr << "Warning: Scene does not exist: " << sceneName << std::endl;
			return;
		}
	}
	if (endCurrentScene)
	{
		m_sceneMap.erase(m_currentScene);
	}
	m_currentScene = sceneName;
}

std::shared_ptr<Scene> GameEngine::currentScene()
{
	return m_sceneMap.at(m_currentScene);
}

void GameEngine::update()
{
	if (!isRunning()) { return; }
	if (m_sceneMap.empty()) { return; }

	sUserInput();
	currentScene()->simulate(m_simulationSpeed);
	currentScene()->sRender();
	m_window.display();
}

void GameEngine::run()
{
	while (isRunning())
	{
		update();
	}
}

void GameEngine::quit()
{
	m_running = false;
}

void GameEngine::sUserInput()
{
	sf::Event event;
	while (m_window.pollEvent(event))
	{
		// the x button was clicked on the window
		if (event.type == sf::Event::Closed)
		{
			quit();
		}
		// keyboard actions
		if (event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased)
		{
			if (currentScene()->getActionMap().find(event.key.code) == currentScene()->getActionMap().end())
			{
				continue;
			}
			const std::string actionType = (event.type == sf::Event::KeyPressed) ? "START" : "END";
			currentScene()->doAction(Action(currentScene()->getActionMap().at(event.key.code), actionType));
		}
		// mouse actions
		auto mpos = sf::Mouse::getPosition(m_window);
		Vec2 pos(static_cast<float>(mpos.x), static_cast<float>(mpos.y));
		if (event.type == sf::Event::MouseButtonPressed)
		{
			switch (event.mouseButton.button)
			{
			case sf::Mouse::Left: { currentScene()->doAction(Action("LEFT_CLICK", "START", pos)); break; }
			case sf::Mouse::Right: { currentScene()->doAction(Action("RIGHT_CLICK", "START", pos)); break; }
			case sf::Mouse::Middle: { currentScene()->doAction(Action("MIDDLE_CLICK", "START", pos)); break; }
			default: break;
			}
		}
		else if (event.type == sf::Event::MouseButtonReleased)
		{
			switch (event.mouseButton.button)
			{
			case sf::Mouse::Left: { currentScene()->doAction(Action("LEFT_CLICK", "END", pos)); break; }
			case sf::Mouse::Right: { currentScene()->doAction(Action("RIGHT_CLICK", "END", pos)); break; }
			case sf::Mouse::Middle: { currentScene()->doAction(Action("MIDDLE_CLICK", "END", pos)); break; }
			default: break;
			}
		}
		else if (event.type == sf::Event::MouseMoved)
		{
			currentScene()->doAction(Action("MOUSE_MOVE", pos));
		}
	}
}

void GameEngine::playSound(const std::string& soundName)
{
	m_assets.getSound(soundName).play();
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
