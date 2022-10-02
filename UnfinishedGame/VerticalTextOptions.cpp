#include "VerticalTextOptions.h"

GUI::VerticalTextOptions::VerticalTextOptions(const sf::Color& unselectedColor, const sf::Color& selectedColor)
	: VerticalList<Text>(), m_unselectedColor(unselectedColor), m_selectedColor(selectedColor) {}

void GUI::VerticalTextOptions::setItems(
	const std::vector<std::pair<std::string, std::shared_ptr<Text>>>& items)
{
	VerticalList::setItems(items);
	if (!m_items.empty())
	{
		select(0);
	}
}

std::pair<std::string, std::shared_ptr<GUI::Text>> GUI::VerticalTextOptions::selected() const
{
	assert(!m_items.empty());
	return m_items[m_selected];
}

void GUI::VerticalTextOptions::select(size_t position)
{
	assert(!m_items.empty());

	m_selected = position % m_items.size();
	
	// Select current text and unselect all others
	for (size_t i = 0; i < m_items.size(); ++i)
	{
		if (i == m_selected)
		{
			m_items[i].second->setFillColor(m_selectedColor);
		}
		else
		{
			m_items[i].second->setFillColor(m_unselectedColor);
		}
	}
}

void GUI::VerticalTextOptions::selectPrevious()
{
	select((m_selected + m_items.size()) - 1);
}

void GUI::VerticalTextOptions::selectNext()
{
	select(m_selected + 1);
}

void GUI::VerticalTextOptions::setSelectedColor(const sf::Color& color)
{
	m_selectedColor = color;
	// update colors
	if (!m_items.empty())
	{
		select(m_selected);
	}
}