#include "CycleableText.h"

GUI::CycleableText::CycleableText() {}

GUI::CycleableText::CycleableText(const std::vector<std::pair<std::string, sf::Color>>& options, size_t state)
	: m_options(options)
{
	setState(state);
}

GUI::CycleableText::CycleableText(const CycleableText& other)
	: m_options(other.m_options), Text(other)
{
	setState(other.m_state);
}

void GUI::CycleableText::nextState()
{
	setState(m_state + 1);
}

void GUI::CycleableText::setState(size_t state)
{
	assert(m_options.size() > 0);
	m_state = state % m_options.size();
	m_text.setString(m_options[m_state].first);
	m_text.setFillColor(m_options[m_state].second);
}

void GUI::CycleableText::setOptions(const std::vector<std::pair<std::string, sf::Color>>& options)
{
	m_options = options;
}

size_t GUI::CycleableText::state() const
{
	return m_state;
}

const std::vector<std::pair<std::string, sf::Color>>& GUI::CycleableText::options() const
{
	return m_options;
}