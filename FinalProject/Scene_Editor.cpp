#include "Scene_Editor.h"

Scene_Editor::Scene_Editor(GameEngine* gameEngine)
	: Scene(gameEngine)
{
	init();
}

void Scene_Editor::init()
{
	registerAction(sf::Keyboard::Escape, "QUIT");
	registerAction(sf::Keyboard::P, "PAUSE");
	registerAction(sf::Keyboard::T, "TOGGLE_TEXTURE");
	registerAction(sf::Keyboard::C, "TOGGLE_COLLISION");

	auto e = m_entityManager.addEntity("tile");
	e.addComponent<CAnimation>(m_game->assets().getAnimation("Block"), true);
	e.addComponent<CTransform>(Vec2(100.0f, 100.0f));
	e = m_entityManager.addEntity("tile");
	e.addComponent<CAnimation>(m_game->assets().getAnimation("Block"), true);
	e.addComponent<CTransform>(Vec2(100.0f, 500.0f));
	e = m_entityManager.addEntity("tile");
	e.addComponent<CAnimation>(m_game->assets().getAnimation("Block"), true);
	e.addComponent<CTransform>(Vec2(800.0f, 100.0f));
	e = m_entityManager.addEntity("tile");
	e.addComponent<CAnimation>(m_game->assets().getAnimation("Block"), true);
	e.addComponent<CTransform>(Vec2(800.0f, 500.0f));

	m_entityManager.update();

	// compute line segments
	m_segments.clear();

	float windowWidth = static_cast<float>(width());
	float windowHeight = static_cast<float>(height());
	m_segments.emplace_back(Vec2(0.0f, 0.0f), Vec2(windowWidth, 0.0f));
	m_segments.emplace_back(Vec2(0.0f, 0.0f), Vec2(0.0f, windowHeight));
	m_segments.emplace_back(Vec2(0.0f, windowHeight), Vec2(windowWidth, windowHeight));
	m_segments.emplace_back(Vec2(windowWidth, 0.0f), Vec2(windowWidth, windowHeight));

	for (auto e : m_entityManager.getEntities())
	{
		if (!e.hasComponent<CAnimation>() || !e.hasComponent<CTransform>())
		{
			continue;
		}
		Vec2 pos = e.getComponent<CTransform>().pos;
		Vec2 halfSize = e.getComponent<CAnimation>().animation.getSize() / 2.0f;
		Vec2 topLeft(pos.x - halfSize.x, pos.y - halfSize.y);
		Vec2 topRight(pos.x + halfSize.x, pos.y - halfSize.y);
		Vec2 btmLeft(pos.x - halfSize.x, pos.y + halfSize.y);
		Vec2 btmRight(pos.x + halfSize.x, pos.y + halfSize.y);
		m_segments.emplace_back(topLeft, topRight);
		m_segments.emplace_back(topRight, btmRight);
		m_segments.emplace_back(btmRight, btmLeft);
		m_segments.emplace_back(btmLeft, topLeft);
	}

	m_mousePos = Vec2(0.0f, 0.0f);
}

void Scene_Editor::update()
{
	m_entityManager.update();

	if (m_paused)
	{

	}
	else
	{
		sAnimation();
	}

	m_currentFrame++;
}

void Scene_Editor::onEnd()
{
	m_game->quit();
	//m_game->changeScene("MENU", nullptr, true);
}

void Scene_Editor::sDoAction(const Action& action)
{
	if (action.type() == "START")
	{
		if (action.name() == "PAUSE") { setPaused(!m_paused); }
		else if (action.name() == "QUIT") { onEnd(); }
		else if (action.name() == "TOGGLE_TEXTURE") { m_drawTextures = !m_drawTextures; }
		else if (action.name() == "TOGGLE_COLLISION") { m_drawCollision = !m_drawCollision; }
	}
	else if (action.type() == "END")
	{

	}
	else if (action.name() == "MOUSE_MOVE")
	{
		// difference in x value between the window's POV and world's POV
		float xDiff = m_game->window().getView().getCenter().x - static_cast<float>(width()) / 2.0f;
		float yDiff = m_game->window().getView().getCenter().y - static_cast<float>(height()) / 2.0f;
		m_mousePos = Vec2(xDiff + action.pos().x, yDiff + action.pos().y);
	}
}

void Scene_Editor::sAnimation()
{
	// destruct entities with non-repeating finished animations
	for (auto e : m_entityManager.getEntities())
	{
		if (!e.hasComponent<CAnimation>()) { continue; }

		auto& animation = e.getComponent<CAnimation>();
		animation.animation.update();
		if (!animation.repeat && animation.animation.hasEnded())
		{
			m_entityManager.destroyEntity(e);
		}
	}
}

void Scene_Editor::drawLine(const Vec2& p1, const Vec2& p2)
{
	sf::Vertex line[] = { sf::Vector2f(p1.x, p1.y), sf::Vector2f(p2.x, p2.y) };
	line[0].color = sf::Color::Red;
	line[1].color = sf::Color::Red;
	m_game->window().draw(line, 2, sf::Lines);
}

void Scene_Editor::sRender()
{
	m_game->window().clear(sf::Color(60, 60, 160));

	// draw all entity textures / animations
	if (m_drawTextures)
	{
		// draw entity animations
		for (auto e : m_entityManager.getEntities())
		{
			if (!e.hasComponent<CAnimation>()) { continue; }

			sf::Color c = sf::Color::White;
			if (e.hasComponent<CInvincibility>())
				c = sf::Color(255, 255, 255, 128);

			auto& transform = e.getComponent<CTransform>();
			auto& animation = e.getComponent<CAnimation>().animation;
			animation.getSprite().setRotation(transform.angle);
			animation.getSprite().setPosition(transform.pos.x, transform.pos.y);
			animation.getSprite().setScale(transform.scale.x, transform.scale.y);
			animation.getSprite().setColor(c);
			m_game->window().draw(animation.getSprite());
		}
	}

	// draw all entity collision bounding boxes with a rectangleshape
	if (m_drawCollision)
	{
		sf::CircleShape dot(4);
		dot.setFillColor(sf::Color::Black);
		for (auto e : m_entityManager.getEntities())
		{
			if (e.hasComponent<CBoundingBox>())
			{
				auto& box = e.getComponent<CBoundingBox>();
				auto& transform = e.getComponent<CTransform>();
				sf::RectangleShape rect;
				rect.setSize(sf::Vector2f(box.size.x - 1, box.size.y - 1));
				rect.setOrigin(sf::Vector2f(box.halfSize.x, box.halfSize.y));
				rect.setPosition(transform.pos.x, transform.pos.y);
				rect.setFillColor(sf::Color(0, 0, 0, 0));

				if (box.blockMove && box.blockVision) { rect.setOutlineColor(sf::Color::Black); }
				else if (box.blockMove && !box.blockVision) { rect.setOutlineColor(sf::Color::Blue); }
				else if (!box.blockMove && box.blockVision) { rect.setOutlineColor(sf::Color::Red); }
				else if (!box.blockMove && !box.blockVision) { rect.setOutlineColor(sf::Color::White); }
				rect.setOutlineThickness(1);
				m_game->window().draw(rect);
			}
		}
	}

	// draw visibility polygon
	auto polygon = Physics::visibility_polygon(m_mousePos, m_segments.begin(), m_segments.end());

	for (size_t i = 0; i < polygon.size(); i++)
	{
		auto& point = polygon[i];
		auto& nextPoint = i + 1 == polygon.size() ? polygon[0] : polygon[i+1];

		sf::ConvexShape shape;
		shape.setPointCount(3);
		shape.setFillColor(sf::Color(0, 0, 0, 150));
		shape.setPoint(0, sf::Vector2f(m_mousePos.x, m_mousePos.y));
		shape.setPoint(1, sf::Vector2f(point.x, point.y));
		shape.setPoint(2, sf::Vector2f(nextPoint.x, nextPoint.y));
		m_game->window().draw(shape);
	}

	sf::CircleShape dot(4);
	dot.setPointCount(8);
	dot.setFillColor(sf::Color::Red);
	dot.setOrigin(dot.getRadius(), dot.getRadius());

	for (size_t i = 0; i < polygon.size(); i++)
	{
		auto& point = polygon[i];
		auto& nextPoint = i + 1 == polygon.size() ? polygon[0] : polygon[i + 1];

		dot.setPosition(sf::Vector2f(point.x, point.y));
		m_game->window().draw(dot);

		drawLine(m_mousePos, point);
		drawLine(point, nextPoint);
	}


}