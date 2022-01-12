#include "Scene_Editor.h"

Scene_Editor::Scene_Editor(GameEngine* gameEngine)
	: Scene(gameEngine)
{
	init();
}

void Scene_Editor::init()
{
	registerAction(sf::Keyboard::Escape, "QUIT");
	registerAction(sf::Keyboard::T, "TOGGLE_TEXTURE");
	registerAction(sf::Keyboard::C, "TOGGLE_COLLISION");
	registerAction(sf::Keyboard::L, "TOGGLE_VISIBILITY");

	m_mousePos = Vec2(0.0f, 0.0f);
	
	m_camera = m_entityManager.addEntity("camera");
	m_camera.addComponent<CTransform>(Vec2(static_cast<float>(width()) / 2.0f, static_cast<float>(height()) / 2.0f));
	m_camera.addComponent<CInput>();

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
}

void Scene_Editor::update()
{
	m_entityManager.update();

	sMovement();
	sDragging();
	sAnimation();
	sCamera();

	m_currentFrame++;
}

void Scene_Editor::onEnd()
{
	m_game->changeScene("MENU", nullptr, true);
}

void Scene_Editor::sDoAction(const Action& action)
{
	if (action.type() == "START")
	{
		if (action.name() == "QUIT") { onEnd(); }
		else if (action.name() == "TOGGLE_TEXTURE") { m_drawTextures = !m_drawTextures; }
		else if (action.name() == "TOGGLE_COLLISION") { m_drawCollision = !m_drawCollision; }
		else if (action.name() == "UP") { m_camera.getComponent<CInput>().up = true; }
		else if (action.name() == "DOWN") { m_camera.getComponent<CInput>().down = true; }
		else if (action.name() == "LEFT") { m_camera.getComponent<CInput>().left = true; }
		else if (action.name() == "RIGHT") { m_camera.getComponent<CInput>().right = true; }
		else if (action.name() == "LEFT_CLICK")
		{
			Vec2 viewTopLeft(
				m_game->window().getView().getCenter().x - static_cast<float>(width()) / 2.0f,
				m_game->window().getView().getCenter().y - static_cast<float>(height()) / 2.0f);
			m_mousePos = viewTopLeft + action.pos();
			
			for (auto& e : m_entityManager.getEntities())
			{
				if (e.hasComponent<CDraggable>() && Physics::IsInside(m_mousePos, e))
				{
					e.getComponent<CDraggable>().dragging = !e.getComponent<CDraggable>().dragging;
					std::cout << "Entity clicked: " << e.tag() << std::endl;
					break;
				}
			}
		}
	}
	else if (action.type() == "END")
	{
		if (action.name() == "UP") { m_camera.getComponent<CInput>().up = false; }
		else if (action.name() == "DOWN") { m_camera.getComponent<CInput>().down = false; }
		else if (action.name() == "LEFT") { m_camera.getComponent<CInput>().left = false; }
		else if (action.name() == "RIGHT") { m_camera.getComponent<CInput>().right = false; }
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

void Scene_Editor::sMovement()
{
	auto& transform = m_camera.getComponent<CTransform>();
	auto& input = m_camera.getComponent<CInput>();

	transform.velocity = Vec2(0.0f, 0.0f);
	if (input.left) transform.velocity.x -= m_cameraSpeed;
	if (input.right) transform.velocity.x += m_cameraSpeed;
	if (input.up) transform.velocity.y -= m_cameraSpeed;
	if (input.down) transform.velocity.y += m_cameraSpeed;

	for (auto e : m_entityManager.getEntities())
	{
		if (e.hasComponent<CTransform>())
		{
			e.getComponent<CTransform>().pos += e.getComponent<CTransform>().velocity;
		}
	}
}

void Scene_Editor::sDragging()
{
	m_draggingSomething = false;
	// update dragging entities
	for (auto e : m_entityManager.getEntities())
	{
		if (e.hasComponent<CDraggable>() && e.getComponent<CDraggable>().dragging)
		{
			// set the position of the entity to the mouse position
			e.getComponent<CTransform>().pos = Vec2(m_mousePos.x, m_mousePos.y);
			m_draggingSomething = true;
		}
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

void Scene_Editor::sCamera()
{
	sf::View view = m_game->window().getView();
	auto pos = m_camera.getComponent<CTransform>().pos;
	view.setCenter(pos.x, pos.y);
	m_game->window().setView(view);
}

void Scene_Editor::sRender()
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

}