#pragma once

#include "Text.h"
#include "VerticalList.h"

namespace GUI
{
	class VerticalTextOptions : public VerticalList<Text>
	{
	protected:
		size_t m_selected = 0;
		sf::Color m_unselectedColor, m_selectedColor;

	public:
		VerticalTextOptions(
			const sf::Color& unselectedColor = sf::Color::White,
			const sf::Color& selectedColor = sf::Color::Cyan);

		void setItems(const std::vector<std::pair<std::string, std::shared_ptr<Text>>>& items);

		std::pair<std::string, std::shared_ptr<Text>> selected() const;
		void select(size_t position);
		void selectPrevious();
		void selectNext();
		void setSelectedColor(const sf::Color& color);
	};
}
