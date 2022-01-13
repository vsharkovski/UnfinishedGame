#include "Scene_Editor.h"

Scene_Editor::Scene_Editor(GameEngine* gameEngine, const std::string& levelPath)
	: Scene(gameEngine)
{
	init(levelPath);
}

void Scene_Editor::init(const std::string& levelPath)
{
	registerAction(sf::Keyboard::Escape, "QUIT");
	registerAction(sf::Keyboard::T, "TOGGLE_TEXTURE");
	registerAction(sf::Keyboard::C, "TOGGLE_COLLISION");
	registerAction(sf::Keyboard::G, "TOGGLE_GUI");
	registerAction(sf::Keyboard::W, "UP");
	registerAction(sf::Keyboard::S, "DOWN");
	registerAction(sf::Keyboard::A, "LEFT");
	registerAction(sf::Keyboard::D, "RIGHT");
	registerAction(sf::Keyboard::Up, "UP");
	registerAction(sf::Keyboard::Down, "DOWN");
	registerAction(sf::Keyboard::Left, "LEFT");
	registerAction(sf::Keyboard::Right, "RIGHT");

	m_levelPath = levelPath;
	loadLevel(levelPath);
	initEditorEntities();
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

void Scene_Editor::initEditorEntities()
{
	m_animationToTag.clear();
	
	m_animationToTag["Block"] = "tile";
	m_animationToTag["Tektite"] = "npc";

	for (auto& kvp : m_animationToTag)
	{
		auto e = m_entityManager.addEntity("editoritem");
		e.addComponent<CAnimation>(m_game->assets().getAnimation(kvp.first), true);
		e.addComponent<CTransform>();
		e.addComponent<CClickable>();
	}
}

void Scene_Editor::loadLevel(const std::string& path)
{
	m_entityManager = EntityManager();
	m_mousePos = Vec2(0.0f, 0.0f);

	m_camera = m_entityManager.addEntity("camera");
	m_camera.addComponent<CTransform>(Vec2(static_cast<float>(width()) / 2.0f, static_cast<float>(height()) / 2.0f));
	m_camera.addComponent<CInput>();

	std::ifstream file(path);
	std::string str;
	while (file.good())
	{
		file >> str;
		if (str == "player")
		{
		}
		else if (str == "tile")
		{
			Vec2 pos;
			int blockMove, blockVision;
			file >> str >> blockMove >> blockVision >> pos.x >> pos.y;

			auto entity = m_entityManager.addEntity("tile");
			entity.addComponent<CAnimation>(m_game->assets().getAnimation(str), true);
			entity.addComponent<CTransform>(pos);
			entity.addComponent<CBoundingBox>(entity.getComponent<CAnimation>().animation.getSize(),
				blockMove == 1, blockVision == 1);
			entity.addComponent<CDraggable>();
			entity.addComponent<CClickable>();
		}
		else if (str == "npc")
		{
			Vec2 pos;
			int blockMove, blockVision;
			std::string ai;
			file >> str >> blockMove >> blockVision >> pos.x >> pos.y;

			auto entity = m_entityManager.addEntity("npc");
			entity.addComponent<CAnimation>(m_game->assets().getAnimation(str), true);
			entity.addComponent<CTransform>(pos);
			entity.addComponent<CBoundingBox>(entity.getComponent<CAnimation>().animation.getSize(),
				blockMove == 1, blockVision == 1);
			entity.addComponent<CDraggable>();
			entity.addComponent<CClickable>();
		}
		else
		{
			std::cerr << "Unknown Entity type: " << str << std::endl;
		}
	}
}

void Scene_Editor::saveLevel(const std::string& path)
{

}

void Scene_Editor::sDoAction(const Action& action)
{
	if (action.type() == "START")
	{
		if (action.name() == "QUIT") { onEnd(); }
		else if (action.name() == "TOGGLE_TEXTURE") { m_drawTextures = !m_drawTextures; }
		else if (action.name() == "TOGGLE_COLLISION") { m_drawCollision = !m_drawCollision; }
		else if (action.name() == "TOGGLE_GUI") { m_drawGUI = !m_drawGUI; }
		else if (action.name() == "UP") { m_camera.getComponent<CInput>().up = true; }
		else if (action.name() == "DOWN") { m_camera.getComponent<CInput>().down = true; }
		else if (action.name() == "LEFT") { m_camera.getComponent<CInput>().left = true; }
		else if (action.name() == "RIGHT") { m_camera.getComponent<CInput>().right = true; }
		else if (action.name() == "COPY") { m_camera.getComponent<CInput>().modifier = true; }
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
		else if (action.name() == "COPY") { m_camera.getComponent<CInput>().modifier = false; }
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
					m_draggingCount--;
				}
			}
			else if (m_draggingCount == 0)
			{
				// not dragging this, not dragging anything else, so start dragging
				drag.dragging = true;
				m_draggingCount++;
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
	bool holdingCTRL = m_camera.getComponent<CInput>().modifier;
	for (auto entity : m_entityManager.getEntities())
	{
		if (!entity.hasComponent<CClickable>()) { continue; }
		auto& click = entity.getComponent<CClickable>();

		if (click.leftClicked)
		{
			if (entity.tag() == "editoritem" && m_draggingCount == 0)
			{
				// create a copy and start dragging it
				std::string animName = entity.getComponent<CAnimation>().animation.getName();
				auto e = m_entityManager.addEntity(m_animationToTag[animName]);
				e.addComponent<CAnimation>(m_game->assets().getAnimation(animName), true);
				e.addComponent<CTransform>(m_mousePos);
				e.addComponent<CBoundingBox>(e.getComponent<CAnimation>().animation.getSize());
				e.addComponent<CClickable>();
				e.addComponent<CDraggable>(true); // we are DRAGGING it
				m_draggingCount++;
			}
		}
		if (click.rightClicked)
		{
			if (entity.tag() != "editoritem")
			{
				if (!entity.hasComponent<CDraggable>() || !entity.getComponent<CDraggable>().dragging)
				{
					// only delete entity if it is not being dragged
					m_entityManager.destroyEntity(entity);
				}
			}
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

	// draw all entities except the dragging ones
	for (auto e : m_entityManager.getEntities())
	{
		if (e.tag() != "editoritem" && (!e.hasComponent<CDraggable>() || !e.getComponent<CDraggable>().dragging))
		{
			if (m_drawTextures)
				sRenderDrawEntity(e);
			if (m_drawCollision)
				sRenderDrawCollision(e);
		}
	}

	// GUI
	if (m_drawGUI)
	{
		Vec2 windowSize(static_cast<float>(width()), static_cast<float>(height()));
		Vec2 viewCenter(m_game->window().getView().getCenter().x, m_game->window().getView().getCenter().y);
		Vec2 viewPos = viewCenter - (windowSize / 2.0f); // top left corner

		Vec2 menuSize(200.0f, windowSize.y);
		Vec2 menuPos(viewPos.x + windowSize.x - menuSize.x, viewPos.y);

		sf::RectangleShape background;
		background.setSize(sf::Vector2f(menuSize.x, menuSize.y));
		background.setPosition(sf::Vector2f(menuPos.x, menuPos.y));
		background.setFillColor(sf::Color(0, 0, 0, 255));
		m_game->window().draw(background);

		float nextTopY = menuPos.y + 32.0f;

		for (auto e : m_entityManager.getEntities("editoritem"))
		{
			auto& transform = e.getComponent<CTransform>();
			auto& animation = e.getComponent<CAnimation>().animation;

			// move into menu
			transform.pos.x = menuPos.x + menuSize.x / 2.0f;
			transform.pos.y = nextTopY + animation.getSize().y / 2.0f;

			nextTopY += animation.getSize().y + 32.0f;

			// draw other stuff
			// ignore the texture/collision toggles
			sRenderDrawEntity(e);
			sRenderDrawCollision(e);
		}
	}

	// draw dragging entities
	for (auto e : m_entityManager.getEntities())
	{
		if (e.hasComponent<CDraggable>() && e.getComponent<CDraggable>().dragging)
		{
			if (m_drawTextures)
				sRenderDrawEntity(e);
			if (m_drawCollision)
				sRenderDrawCollision(e);
		}
	}
}

void Scene_Editor::sRenderDrawEntity(Entity e)
{
	if (!e.hasComponent<CAnimation>()) { return; }

	auto& transform = e.getComponent<CTransform>();
	auto& sprite = e.getComponent<CAnimation>().animation.getSprite();
	sprite.setRotation(transform.angle);
	sprite.setPosition(transform.pos.x, transform.pos.y);
	sprite.setScale(transform.scale.x, transform.scale.y);
	m_game->window().draw(sprite);
}

void Scene_Editor::sRenderDrawCollision(Entity e)
{
	if (!e.hasComponent<CBoundingBox>()) { return; }

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