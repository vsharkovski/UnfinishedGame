#pragma once

#include "Text.h"
#include "VerticalList.h"

namespace GUI
{
	class VerticalTextOptions : public VerticalList<Text>
	{
	protected:
		bool m_hasSelectedBefore = false;
		size_t m_selected = 0;
		sf::Color m_selectedColor, m_selectedOriginalColor;

	public:
		VerticalTextOptions(const sf::Color& selectedColor);

		void setItems(const std::vector<std::pair<std::string, std::shared_ptr<Text>>>& items);

		std::pair<std::string, std::shared_ptr<Text>> selected() const;
		void select(size_t position);
		void selectPrevious();
		void selectNext();
		void setSelectedColor(const sf::Color& color);
	};
}
