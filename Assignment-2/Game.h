#pragma once
#include <SFML/Graphics.hpp>
#include "EntityManager.h"
#include "Entity.h"

class Game
{
	sf::RenderWindow m_window;
	EntityManager m_entities;
	Entity m_player;
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

