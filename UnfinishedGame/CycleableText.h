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
		CycleableText();
		CycleableText(const std::vector<std::pair<std::string, sf::Color>>& options, size_t state = 0);
		CycleableText(const CycleableText& other);

		void nextState();
		void setState(size_t state);
		void setOptions(const std::vector<std::pair<std::string, sf::Color>>& options);

		size_t state() const;
		const std::vector<std::pair<std::string, sf::Color>>& options() const;
	};
}
