#pragma once
#include "Common.h"

class ParticleSystem
{
	struct Particle
	{
		sf::Vector2f velocity;
		unsigned int lifespan = 0;
	};

	std::vector<Particle>	m_particles;
	sf::VertexArray			m_vertices;
	sf::Vector2u			m_windowSize;
	float					m_size = 8.0f;

	std::mt19937 m_randomEngine;
	std::uniform_real_distribution<float> m_randomVelocityDist;
	std::uniform_int_distribution<int> m_randomLifespanDist;
	std::uniform_int_distribution<int> m_randomColorDist;
	
	void resetParticle(size_t index, bool first = false);

public:
	ParticleSystem();
	void init(size_t width, size_t height);
	void update();
	void draw(sf::RenderWindow& window);
	void resetParticles(size_t count = 1024, float size = 8.0f);
};

