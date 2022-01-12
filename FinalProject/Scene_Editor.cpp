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
	registerAction(sf::Keyboard::W, "UP");
	registerAction(sf::Keyboard::Up, "UP");
	registerAction(sf::Keyboard::S, "DOWN");
	registerAction(sf::Keyboard::Down, "DOWN");
	registerAction(sf::Keyboard::A, "LEFT");
	registerAction(sf::Keyboard::Left, "LEFT");
	registerAction(sf::Keyboard::D, "RIGHT");
	registerAction(sf::Keyboard::Right, "RIGHT");

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
		e.addComponent<CClickable>();
	}

	auto e = m_entityManager.addEntity("npc");
	e.addComponent<CAnimation>(m_game->assets().getAnimation("Tektite"), true);
	e.addComponent<CTransform>(Vec2(static_cast<float>(width()) / 2.0f, static_cast<float>(height()) / 2.0f));
	e.addComponent<CBoundingBox>(e.getComponent<CAnimation>().animation.getSize(), false, false);
	e.addComponent<CClickable>();
}

void Scene_Editor::update()
{
	m_entityManager.update();

	sMovement();
	sDragging();
	sClicking();
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
			
			for (auto e : m_entityManager.getEntities())
			{
				if (e.hasComponent<CClickable>() && Physics::IsInside(m_mousePos, e))
				{
					std::cout << "Entity left clicked: " << e.tag() << " " << e.id() << std::endl;
					e.getComponent<CClickable>().leftClicked = true;
					break;
				}
			}
		}
		else if (action.name() == "RIGHT_CLICK")
		{
			Vec2 viewTopLeft(
				m_game->window().getView().getCenter().x - static_cast<float>(width()) / 2.0f,
				m_game->window().getView().getCenter().y - static_cast<float>(height()) / 2.0f);
			m_mousePos = viewTopLeft + action.pos();

			for (auto e : m_entityManager.getEntities())
			{
				if (e.hasComponent<CClickable>() && Physics::IsInside(m_mousePos, e))
				{
					std::cout << "Entity right clicked: " << e.tag() << " " << e.id() << std::endl;
					e.getComponent<CClickable>().rightClicked = true;
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
	// update dragging entities
	for (auto e : m_entityManager.getEntities())
	{
		if (!e.hasComponent<CClickable>() || !e.hasComponent<CDraggable>()) { continue; }
		auto& drag = e.getComponent<CDraggable>();

		if (e.getComponent<CClickable>().leftClicked)
		{
			if (drag.dragging)
			{
				// already dragging this, so try to place it down
				e.getComponent<CTransform>().pos = m_mousePos;

				bool colliding = false;
				for (auto other : m_entityManager.getEntities())
				{
					if (other.id() == e.id()) { continue; }
					if (other.hasComponent<CDraggable>() && other.getComponent<CDraggable>().dragging) { continue; }
					Vec2 overlap = Physics::GetOverlap(e, other);
					if (overlap.x > 0.0f && overlap.y > 0.0f)
					{
						colliding = true;
						break;
					}
				}
				if (colliding)
				{
					// overlapping with something else
					
				}
				else
				{
					// not overlapping, can place down
					drag.dragging = false;
					m_draggingSomething = false;
				}
			}
			else if (!m_draggingSomething)
			{
				// not dragging this, not dragging anything else, so start dragging
				drag.dragging = true;
				m_draggingSomething = true;
			}
		}
		
		if (drag.dragging)
		{
			e.getComponent<CTransform>().pos = m_mousePos;
		}
	}
}

void Scene_Editor::sClicking()
{
	for (auto e : m_entityManager.getEntities())
	{
		if (!e.hasComponent<CClickable>()) { continue; }
		auto& click = e.getComponent<CClickable>();

		if (click.rightClicked)
		{
			m_entityManager.destroyEntity(e);
		}

		// relax for next frame
		click.leftClicked = false;
		click.rightClicked = false;
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