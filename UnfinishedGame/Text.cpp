#include "Text.h"

GUI::Text::Text() {}

GUI::Text::Text(const sf::Text& text, const std::string& initialString)
	: m_text(text.getString(), *text.getFont(), text.getCharacterSize())
{
	m_text.setFillColor(text.getFillColor());
	m_text.setString(initialString);
	m_spacing = static_cast<float>(text.getCharacterSize()) + DEFAULT_SPACING;
}

GUI::Text::Text(const Text& other)
	: Text(other.m_text) {};

void GUI::Text::setPosition(const Vec2& position)
{
	m_text.setPosition(position.x, position.y);
}

Vec2 GUI::Text::position() const
{
	auto pos = m_text.getPosition();
	return Vec2(pos.x, pos.y);
}

Vec2 GUI::Text::positionUnder() const
{
	auto pos = position();
	return Vec2(pos.x, pos.y + m_spacing);
}

void GUI::Text::draw(sf::RenderWindow& window) const
{
	window.draw(m_text);
}

void GUI::Text::setString(const std::string& string)
{
	m_text.setString(string);
}

void GUI::Text::setFillColor(const sf::Color& color)
{
	m_text.setFillColor(color);
}

void GUI::Text::setFont(const sf::Font& font)
{
	m_text.setFont(font);
}

void GUI::Text::setCharacterSize(unsigned int size)
{
	m_text.setCharacterSize(size);
	m_spacing = static_cast<float>(size) + DEFAULT_SPACING;
}

const sf::Color& GUI::Text::getFillColor() const
{
	return m_text.getFillColor();
}