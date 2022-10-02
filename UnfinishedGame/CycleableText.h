#pragma once

#include "Text.h"

namespace GUI
{
	class CycleableText : public Text
	{
	protected:
		std::vector<std::pair<std::string, sf::Color>> m_options;
		size_t m_state = 0;

	public:
		CycleableText() {}

		CycleableText(const std::vector<std::pair<std::string, sf::Color>>& options, size_t state = 0)
			: m_options(options)
		{
			setState(state);
		}
		
		CycleableText(const CycleableText& other)
			: m_options(other.m_options)
		{
			setState(other.m_state);
		}

		void nextState()
		{
			setState(m_state + 1);
		}

		void setState(size_t state)
		{
			assert(m_options.size() > 0);
			m_state = state % m_options.size();
			m_text.setString(m_options[m_state].first);
			m_text.setFillColor(m_options[m_state].second);
		}

		void setOptions(const std::vector<std::pair<std::string, sf::Color>>& options)
		{
			m_options = options;
		}

		size_t state() const
		{
			return m_state;
		}

		const std::vector<std::pair<std::string, sf::Color>>& options() const
		{
			return m_options;
		}
	};
}
