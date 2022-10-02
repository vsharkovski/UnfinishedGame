#include "Text.h"

GUI::Text::Text() {}

GUI::Text::Text(const sf::Text& text)
	: m_text(text) {}

GUI::Text::Text(const Text& other)
	: m_text(other.m_text) {};

void GUI::Text::setPosition(const Vec2& position)
{
	m_text.setPosition(position.x, position.y);
}

Vec2 GUI::Text::position() const
{
	auto bounds = m_text.getGlobalBounds();
	return Vec2(bounds.left, bounds.top);
}

Vec2 GUI::Text::positionUnder() const
{
	auto bounds = m_text.getGlobalBounds();
	return Vec2(
		bounds.left,
		bounds.top + bounds.height
	);
}

void GUI::Text::draw(sf::RenderWindow& window)
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
}
