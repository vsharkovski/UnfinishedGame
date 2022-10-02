#pragma once

#include "Common.h"

namespace GUI
{
	class Text
	{
	protected:
		sf::Text m_text;

	public:
		Text() {}

		Text(const sf::Text& text)
			: m_text(text) {}

		Text(const Text& other)
			: m_text(other.m_text) {};

		sf::Text& text()
		{
			return m_text;
		}

		sf::Vector2f getPositionUnder() const
		{
			auto bounds = m_text.getGlobalBounds();
			return sf::Vector2f(
				bounds.left,
				bounds.top + bounds.height
			);
		}

	};
}
