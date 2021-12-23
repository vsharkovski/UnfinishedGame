#include "GameEngine.h"

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