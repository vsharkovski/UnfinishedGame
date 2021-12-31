#include "Animation.h"

Animation::Animation()
{

}

Animation::Animation(const std::string& name, const sf::Texture& t)
	: m_name(name), m_sprite(t)
{
	m_size = Vec2(static_cast<float>(t.getSize().x) / m_frameCount, static_cast<float>(t.getSize().y));
	m_sprite.setOrigin(m_size.x / 2.0f, m_size.y / 2.0f);
	m_sprite.setTextureRect(sf::IntRect(static_cast<int>(m_currentFrame * m_size.x), 0, static_cast<int>(m_size.x), static_cast<int>(m_size.y)));
}

Animation::Animation(const std::string& name, const sf::Texture& t, size_t frameCount, size_t speed)
	: m_name(name), m_sprite(t), m_frameCount(frameCount), m_speed(speed)
{
	m_size = Vec2(static_cast<float>(t.getSize().x) / m_frameCount, static_cast<float>(t.getSize().y));
	m_sprite.setOrigin(m_size.x / 2.0f, m_size.y / 2.0f);
	m_sprite.setTextureRect(sf::IntRect(static_cast<int>(m_currentFrame * m_size.x), 0, static_cast<int>(m_size.x), static_cast<int>(m_size.y)));
}

void Animation::update()
{
	m_currentFrame++;
	if (m_speed > 0 && m_frameCount > 1)
	{
		size_t frame = (m_currentFrame / m_speed) % m_frameCount;
		m_sprite.setTextureRect(sf::IntRect(static_cast<int>(m_currentFrame * m_size.x), 0, static_cast<int>(m_size.x), static_cast<int>(m_size.y)));
	}
}

bool Animation::hasEnded() const
{
	return (m_speed == 0) || (m_currentFrame / m_speed) >= m_frameCount;
}

const std::string& Animation::getName() const
{
	return m_name;
}

const Vec2& Animation::getSize() const
{
	return m_size;
}

sf::Sprite& Animation::getSprite()
{
	return m_sprite;
}
