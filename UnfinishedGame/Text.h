#pragma once

#include "Common.h"
#include "GuiItem.h"

namespace GUI
{
	class Text : public GuiItem
	{
	protected:
		const float DEFAULT_SPACING = 4.0f;
		float m_spacing = DEFAULT_SPACING;

		sf::Text m_text;

	public:
		Text();
		Text(const sf::Text& text, const std::string& initialString = "");
		Text(const Text& other);

		void setPosition(const Vec2& position);
		Vec2 position() const;
		Vec2 positionUnder() const;
		void draw(sf::RenderWindow& window) const;

		void setString(const std::string& string);
		void setFillColor(const sf::Color& color);
		void setFont(const sf::Font& font);
		void setCharacterSize(unsigned int size);

		const sf::Color& getFillColor() const;

		Text& operator=(const Text& other);
	};
}
