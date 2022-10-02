#pragma once

namespace GUI
{
	class GuiItem
	{
	protected:
		GuiItem() {}

	public:
		virtual void setPosition(const Vec2& position) = 0;
		virtual Vec2 position() const = 0;
		virtual Vec2 positionUnder() const = 0;
		virtual void draw(sf::RenderWindow& window) const = 0;
	};
}