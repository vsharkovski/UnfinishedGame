#include "Scene.h"
#include "GameEngine.h"

Scene::Scene()
{
	
}

Scene::Scene(GameEngine* gameEngine)
	: m_game(gameEngine)
{

}

void Scene::setPaused(bool paused)
{
	m_paused = paused;
}

size_t Scene::width() const
{
	return m_game->window().getSize().x;
}

size_t Scene::height() const
{
	return m_game->window().getSize().y;
}

size_t Scene::currentFrame() const
{
	return m_currentFrame;
}

void Scene::simulate(const int frames)
{

}

void Scene::doAction(const Action& action)
{

}

void Scene::registerAction(const int key, const std::string& actionName)
{

}