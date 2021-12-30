#pragma once

#include "Common.h"
#include "GameEngine.h"
#include "EntityManager.h"
#include "Action.h"

class Scene
{
	GameEngine* m_game = nullptr;
	EntityManager m_entityManager;
	std::map<int, std::string> m_actionMap;
	int m_currentFrame = 0;
	bool m_paused = false;
	bool m_hasEnded = false;
	
	Scene();
	Scene(GameEngine* gameEngine);

	virtual void update() = 0;
	virtual void sDoAction(const Action& action) = 0;
	virtual void sRender() = 0;

	void simulate(const int frames);
	void doAction(const Action& action);
	void registerAction(const int key, const std::string& actionName);

public:
	void setPaused(bool paused);
	size_t width() const;
	size_t height() const;
	size_t currentFrame() const;
};

