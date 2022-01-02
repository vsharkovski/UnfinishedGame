#include "Scene.h"

Scene::Scene()
	: m_game(nullptr)
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

const std::map<int, std::string>& Scene::getActionMap() const
{
	return m_actionMap;
}

void Scene::simulate(const int frames)
{

}

void Scene::doAction(const Action& action)
{
	sDoAction(action);
}

void Scene::registerAction(const int inputKey, const std::string& actionName)
{
	//std::cout << "Registered action key=" << inputKey << " name=" << actionName << std::endl;
	m_actionMap[inputKey] = actionName;
}