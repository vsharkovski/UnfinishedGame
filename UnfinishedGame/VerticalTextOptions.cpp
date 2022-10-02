#include "VerticalTextOptions.h"

GUI::VerticalTextOptions::VerticalTextOptions(const sf::Color& selectedColor)
	: VerticalList<Text>(), m_selectedColor(selectedColor) {}

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
	// Unselect previous text
	if (m_hasSelectedBefore)
	{
		m_items[m_selected].second->setFillColor(m_selectedOriginalColor);
	}
	// Select current text
	m_selected = position % m_items.size();
	m_hasSelectedBefore = true;
	m_selectedOriginalColor = m_items[m_selected].second->getFillColor();
	m_items[m_selected].second->setFillColor(m_selectedColor);
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
	if (m_hasSelectedBefore)
	{
		m_items[m_selected].second->setFillColor(m_selectedColor);
	}
}