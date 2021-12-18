#pragma once
#include <SFML/Graphics.hpp>
#include "EntityManager.h"
#include "Entity.h"

class Game
{
	std::shared_ptr<sf::RenderWindow> m_window;
	sf::Font m_font;
	sf::Color m_fontColor;
	int m_fontSize;


	EntityManager m_entities;
	std::shared_ptr<Entity> m_player;
	bool m_paused;
	bool m_running;

public:
	Game();
	void update();

	// systems
	void sMovement();
	void sUserInput();
	void sRender();
	void sEnemySpawner();
	void sCollision();
};

