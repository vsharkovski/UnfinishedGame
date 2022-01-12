#include "Scene_Level.h"

Scene_Level::Scene_Level(GameEngine* gameEngine)
	: Scene(gameEngine)
{
	init();
}

void Scene_Level::init()
{
	registerAction(sf::Keyboard::Escape, "QUIT");
	registerAction(sf::Keyboard::P, "PAUSE");
	registerAction(sf::Keyboard::T, "TOGGLE_TEXTURE");
	registerAction(sf::Keyboard::C, "TOGGLE_COLLISION");
	registerAction(sf::Keyboard::L, "TOGGLE_VISIBILITY");

	m_visibilityMask.create(width(), height());

	for (int i = 0; i < 8; i++)
	{
		auto e = m_entityManager.addEntity("tile");
		e.addComponent<CAnimation>(m_game->assets().getAnimation("Block"), true);
		e.addComponent<CTransform>(Vec2(100.0f + 100.0f * static_cast<float>(i), static_cast<float>(height()) - 200.0f));
		e.addComponent<CBoundingBox>(e.getComponent<CAnimation>().animation.getSize(), true, true);
		e.addComponent<CDraggable>();
	}

	auto e = m_entityManager.addEntity("npc");
	e.addComponent<CAnimation>(m_game->assets().getAnimation("Tektite"), true);
	e.addComponent<CTransform>(Vec2(static_cast<float>(width()) / 2.0f, static_cast<float>(height()) / 2.0f));
	e.addComponent<CBoundingBox>(e.getComponent<CAnimation>().animation.getSize(), false, false);

	m_mousePos = Vec2(0.0f, 0.0f);
}

void Scene_Level::update()
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

void Scene_Level::onEnd()
{
	m_game->changeScene("MENU", nullptr, true);
}

void Scene_Level::sDoAction(const Action& action)
{
	if (action.type() == "START")
	{
		if (action.name() == "PAUSE") { setPaused(!m_paused); }
		else if (action.name() == "QUIT") { onEnd(); }
		else if (action.name() == "TOGGLE_TEXTURE") { m_drawTextures = !m_drawTextures; }
		else if (action.name() == "TOGGLE_COLLISION") { m_drawCollision = !m_drawCollision; }
		else if (action.name() == "TOGGLE_VISIBILITY") { m_drawVisibility = (m_drawVisibility + 1) % 3; }
	}
	else if (action.type() == "END")
	{

	}
	else if (action.name() == "MOUSE_MOVE")
	{
		// difference in x value between the window's POV and world's POV
		Vec2 viewTopLeft(
			m_game->window().getView().getCenter().x - static_cast<float>(width()) / 2.0f,
			m_game->window().getView().getCenter().y - static_cast<float>(height()) / 2.0f);
		m_mousePos = viewTopLeft + action.pos();
	}
}

void Scene_Level::sAnimation()
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

void Scene_Level::drawLine(const Vec2& p1, const Vec2& p2)
{
	sf::Vertex line[] = { sf::Vector2f(p1.x, p1.y), sf::Vector2f(p2.x, p2.y) };
	line[0].color = sf::Color::Green;
	line[1].color = sf::Color::Green;
	m_game->window().draw(line, 2, sf::Lines);
}

void Scene_Level::computeAllSegments(float offset)
{
	m_segments.clear();

	float windowWidth = static_cast<float>(width());
	float windowHeight = static_cast<float>(height());
	Vec2 viewTopLeft(
		m_game->window().getView().getCenter().x - windowWidth / 2.0f,
		m_game->window().getView().getCenter().y - windowHeight / 2.0f);
	Vec2 viewBtmRight(viewTopLeft.x + windowWidth, viewTopLeft.y + windowHeight);

	m_segments.emplace_back(viewTopLeft, Vec2(viewBtmRight.x, viewTopLeft.y)); // top
	m_segments.emplace_back(viewTopLeft, Vec2(viewTopLeft.x, viewBtmRight.y)); // left
	m_segments.emplace_back(Vec2(viewTopLeft.x, viewBtmRight.y), viewBtmRight); // bottom
	m_segments.emplace_back(Vec2(viewBtmRight.x, viewTopLeft.y), viewBtmRight); // right

	for (auto e : m_entityManager.getEntities())
	{
		if (!e.hasComponent<CAnimation>()
			|| !e.hasComponent<CTransform>()
			|| !e.hasComponent<CBoundingBox>()
			|| !e.getComponent<CBoundingBox>().blockVision)
		{
			continue;
		}
		Vec2 pos = e.getComponent<CTransform>().pos;
		Vec2 size = e.getComponent<CBoundingBox>().halfSize;
		size.x -= offset;
		size.y -= offset;
		Vec2 topLeft(pos.x - size.x, pos.y - size.y);
		Vec2 topRight(pos.x + size.x, pos.y - size.y);
		Vec2 btmLeft(pos.x - size.x, pos.y + size.y);
		Vec2 btmRight(pos.x + size.x, pos.y + size.y);
		m_segments.emplace_back(topLeft, topRight);
		m_segments.emplace_back(topRight, btmRight);
		m_segments.emplace_back(btmRight, btmLeft);
		m_segments.emplace_back(btmLeft, topLeft);
	}
}

void Scene_Level::sRender()
{
	/*
	visibilityMask is a mask, where alpha=255 (opaque) if not in light, and alpha=0 (transparent) otherwise
	*/

	m_game->window().clear(sf::Color(100, 100, 100));

	// draw all entity textures / animations
	if (m_drawTextures)
	{
		// draw entity animations
		for (auto e : m_entityManager.getEntities())
		{
			if (!e.hasComponent<CAnimation>()) { continue; }

			auto& transform = e.getComponent<CTransform>();
			auto& animation = e.getComponent<CAnimation>().animation;
			animation.getSprite().setRotation(transform.angle);
			animation.getSprite().setPosition(transform.pos.x, transform.pos.y);
			animation.getSprite().setScale(transform.scale.x, transform.scale.y);
			m_game->window().draw(animation.getSprite());
		}
	}

	// draw all entity collision bounding boxes with a rectangleshape
	if (m_drawCollision)
	{
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

	if (m_drawVisibility > 0)
	{
		// draw shadows behind objects that are not visible
		m_visibilityMask.clear(sf::Color(0, 0, 0, 255));

		computeAllSegments();
		auto polygon = Physics::visibility_polygon(m_mousePos, m_segments.begin(), m_segments.end());

		sf::Color color(0, 0, 0, 0); // transparent
		std::vector<sf::Vertex> triangleFan(polygon.size() + 2);
		triangleFan[0].position.x = m_mousePos.x;
		triangleFan[0].position.y = m_mousePos.y;
		triangleFan[0].color = color;
		for (size_t i = 0; i <= polygon.size(); i++)
		{
			size_t j = i == polygon.size() ? 0 : i;
			triangleFan[i + 1].position.x = polygon[j].x;
			triangleFan[i + 1].position.y = polygon[j].y;
			triangleFan[i + 1].color = color;
		}
		m_visibilityMask.draw(&triangleFan[0], triangleFan.size(), sf::TriangleFan, sf::BlendNone);

		m_visibilityMask.display();
		m_game->window().draw(sf::Sprite(m_visibilityMask.getTexture()));

		// draw fancy lines and dots
		if (m_drawVisibility > 1)
		{
			sf::CircleShape dot(4);
			dot.setPointCount(8);
			dot.setOrigin(dot.getRadius(), dot.getRadius());

			for (size_t i = 0; i < polygon.size(); i++)
			{
				auto& point = polygon[i];
				auto& nextPoint = i + 1 == polygon.size() ? polygon[0] : polygon[i + 1];

				dot.setPosition(sf::Vector2f(point.x, point.y));
				int change = (255 / polygon.size()) * i;
				dot.setFillColor(sf::Color(std::max(0, 255 - change), change, change));
				m_game->window().draw(dot);

				drawLine(m_mousePos, point);
				drawLine(point, nextPoint);
			}
		}
	}

}