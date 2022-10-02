#pragma once

#include "Common.h"
#include "GuiItem.h"

namespace GUI
{
	class Text : public GuiItem
	{
	protected:
		sf::Text m_text;

	public:
		Text();
		Text(const sf::Text& text);
		Text(const Text& other);

		void setPosition(const Vec2& position);
		Vec2 position() const;
		Vec2 positionUnder() const;
		void draw(sf::RenderWindow& window);

		void setString(const std::string& string);
		void setFillColor(const sf::Color& color);
		void setFont(const sf::Font& font);
		void setCharacterSize(unsigned int size);
	};
}
