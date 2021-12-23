#pragma once

#include <SFML/Graphics.hpp>
#include "Entity.h"
#include "EntityManager.h"

struct PlayerConfig { int SR, CR, FR, FG, FB, OR, OG, OB, OT, V; float S; };
struct EnemyConfig { int SR, CR, OR, OG, OB, OT, VMIN, VMAX, L, SI; float SMIN, SMAX; };
struct BulletConfig { int SR, CR, FR, FG, FB, OR, OG, OB, OT, V, L; float S; };

class Game
{
	sf::RenderWindow	m_window;
	EntityManager		m_entities;
	sf::Font			m_font; // the font we will use to draw
	sf::Text			m_text; // the score text to be drawn to the screen
	PlayerConfig		m_playerConfig;
	EnemyConfig			m_enemyConfig;
	BulletConfig		m_bulletConfig;
	int					m_score = 0;
	int					m_currentFrame = 0;
	int					m_lastEnemySpawnTime = 0;
	int					m_lastSpecialTime = 0;
	const int			m_specialCooldown = 180;
	bool				m_paused = false; // whether we update game logic
	bool				m_running = true; // whether the game is paused

	std::shared_ptr<Entity> m_player;

	void init(const std::string& path);	// initialize the GameState with a config file path
	void setPaused(bool paused);			// pause the game

	void sMovement();		// System: Entity position / movement update
	void sUserInput();		// System: User input
	void sLifespan();		// System: Lifespan
	void sBurstShooting();	// System: Burst shooting
	void sRender();			// System: Render / drawing
	void sEnemySpawner();	// System: Spawns enemies
	void sCollision();		// System: Collisions

	void spawnPlayer();
	void spawnEnemy();
	void spawnSmallEnemies(std::shared_ptr<Entity> enemy);
	void spawnBullet(std::shared_ptr<Entity> entity, const Vec2& target, const bool chill);
	void startBurst(std::shared_ptr<Entity> entity);

public:
	Game(const std::string& config); // constructor, takes in game config
	void run();

};

