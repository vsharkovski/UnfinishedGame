#pragma once

#include "Common.h"
#include "Scene.h"
#include "Assets.h"
#include <memory>

typedef std::map<std::string, std::shared_ptr<Scene>> SceneMap;

class GameEngine
{
protected:
	sf::RenderWindow m_window;
	Assets m_assets;
	SceneMap m_sceneMap;
	std::string m_currentScene;
	size_t simulationSpeed = 1;
	bool m_running = true;

	void init(const std::string& path);
	void update();
	
	void sUserInput();

	std::shared_ptr<Scene> currentScene();

public:
	GameEngine(const std::string& path);

	void changeScene(const std::string& sceneName, std::shared_ptr<Scene> scene, bool endCurrentScene = false);

	void run();
	void quit();
	
	sf::RenderWindow& window();
	const Assets& assets() const;
	const bool isRunning();
};
