#pragma once

#include "Text.h"

namespace GUI
{
	template<
		typename T,
		typename std::enable_if<std::is_base_of<GuiItem, T>::value>::type* = nullptr>
	class VerticalList : public GuiItem
	{
	protected:
		std::vector<std::pair<std::string, std::shared_ptr<T>>> m_items;
		std::map<std::string, size_t> m_itemsMap;

	public:
		VerticalList() {}

		void setPosition(const Vec2& position)
		{
			Vec2 currPosition(position);
			for (auto item : m_items)
			{
				item.second->setPosition(currPosition);
				currPosition = item.second->positionUnder();
			}
		}

		Vec2 position() const
		{
			assert(!m_items.empty());
			return m_items.front().second->position();
		}

		Vec2 positionUnder() const
		{
			assert(!m_items.empty());
			return m_items.back().second->positionUnder();
		}

		void draw(sf::RenderWindow& window) const
		{
			for (auto item : m_items)
				item.second->draw(window);
		}

		void setItems(const std::vector<std::pair<std::string, std::shared_ptr<T>>>& items)
		{
			m_items = items;
			m_itemsMap.clear();
			for (size_t i = 0; i < items.size(); ++i)
			{
				m_itemsMap[items[i].first] = i;
			}
		}

		std::shared_ptr<T> getItemByName(const std::string& name) const
		{
			auto result = m_itemsMap.find(name);
			assert(result != m_itemsMap.end());
			return m_items[result->second].second;
		}
	};
}