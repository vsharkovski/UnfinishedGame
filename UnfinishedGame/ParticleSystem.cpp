#include "ParticleSystem.h"

ParticleSystem::ParticleSystem()
{
	m_randomVelocityDist = std::uniform_real_distribution<float>(-5.0f, 5.0f);
	m_randomLifespanDist = std::uniform_int_distribution<int>(30, 90);
	m_randomColorDist = std::uniform_int_distribution<int>(0, 255);
}

void ParticleSystem::resetParticles(size_t count, float size)
{
	m_particles = std::vector<Particle>(count);
	m_vertices = sf::VertexArray(sf::Quads, count * 4);
	m_size = size;

	for (size_t p = 0; p < m_particles.size(); p++)
	{
		resetParticle(p, true);
	}
}

void ParticleSystem::resetParticle(size_t index, bool first)
{
	// give the particle an initial position
	float mx = static_cast<float>(m_windowSize.x / 2);
	float my = static_cast<float>(m_windowSize.y / 2);
	m_vertices[4 * index + 0].position = sf::Vector2f(mx, my);
	m_vertices[4 * index + 1].position = sf::Vector2f(mx + m_size, my);
	m_vertices[4 * index + 2].position = sf::Vector2f(mx + m_size, my + m_size);
	m_vertices[4 * index + 3].position = sf::Vector2f(mx, my + m_size);

	// give the particle a color
	int randomAlpha = m_randomColorDist(m_randomEngine);
	if (first) randomAlpha = 0; // make it transparent until its first lifespan is done
	sf::Color color(255, 0, 255, randomAlpha);

	m_vertices[4 * index + 0].color = color;
	m_vertices[4 * index + 1].color = color;
	m_vertices[4 * index + 2].color = color;
	m_vertices[4 * index + 3].color = color;

	// give the particle a velocity
	float rx = m_randomVelocityDist(m_randomEngine);
	float ry = m_randomVelocityDist(m_randomEngine);
	m_particles[index].velocity = sf::Vector2f(rx, ry);
	
	// give the particle a lifespan
	m_particles[index].lifespan = m_randomLifespanDist(m_randomEngine);
}

void ParticleSystem::init(size_t width, size_t height)
{
	m_windowSize = sf::Vector2u(width, height);
	resetParticles();
}

void ParticleSystem::update()
{
	for (size_t i = 0; i < m_particles.size(); i++)
	{
		if (m_particles[i].lifespan == 0)
		{
			resetParticle(i);
		}

		m_vertices[4 * i + 0].position += m_particles[i].velocity;
		m_vertices[4 * i + 1].position += m_particles[i].velocity;
		m_vertices[4 * i + 2].position += m_particles[i].velocity;
		m_vertices[4 * i + 3].position += m_particles[i].velocity;

		m_particles[i].lifespan -= 1;
	}
}

void ParticleSystem::draw(sf::RenderWindow& window)
{
	window.draw(m_vertices);
}