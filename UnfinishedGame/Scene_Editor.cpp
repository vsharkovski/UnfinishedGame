#include "Scene_Editor.h"
#include <sstream>

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
	registerAction(sf::Keyboard::LShift, "SHIFT");
	registerAction(sf::Keyboard::W, "UP");
	registerAction(sf::Keyboard::S, "DOWN");
	registerAction(sf::Keyboard::A, "LEFT");
	registerAction(sf::Keyboard::D, "RIGHT");
	registerAction(sf::Keyboard::Up, "UP");
	registerAction(sf::Keyboard::Down, "DOWN");
	registerAction(sf::Keyboard::Left, "LEFT");
	registerAction(sf::Keyboard::Right, "RIGHT");
	registerAction(sf::Keyboard::Num1, "TOGGLE_SELECTED_BLOCK_MOVE");
	registerAction(sf::Keyboard::Num2, "TOGGLE_SELECTED_BLOCK_VISION");
	registerAction(sf::Keyboard::P, "SAVE_LEVEL");

	// Cursors
	if (m_arrowCursor.loadFromSystem(sf::Cursor::Arrow)
		&& m_handCursor.loadFromSystem(sf::Cursor::Hand))
	{
		loadedCursors = true;
		std::cout << "Loaded cursors" << std::endl;
	}

	m_levelPath = levelPath;
	loadLevel(levelPath);

	initGUI();
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

void Scene_Editor::initGUI()
{
	// Menu
	Vec2 windowSize(static_cast<float>(width()), static_cast<float>(height()));
	m_menuSize = Vec2(200.0f, windowSize.y);

	auto& consolasFont = m_game->assets().getFont("Consolas");

	m_menuText.setFont(consolasFont);
	m_menuText.setCharacterSize(12);

	m_blockMoveText.text().setFont(consolasFont);
	m_blockMoveText.text().setCharacterSize(12);
	m_blockMoveText.setOptions({ {"1: Not blocking movement", sf::Color::White}, {"1: Blocking movement", sf::Color::Cyan} });
	m_blockMoveText.setState(m_shouldSelectedBlockMove ? 1 : 0);

	m_blockVisionText.text().setFont(consolasFont);
	m_blockVisionText.text().setCharacterSize(12);
	m_blockVisionText.setOptions({ {"2: Not blocking AI vision", sf::Color::White}, {"2: Blocking AI vision", sf::Color::Cyan} });
	m_blockVisionText.setState(m_shouldSelectedBlockVision ? 1 : 0);

	// GUI entities
	{
		// block
		auto e = m_entityManager.addEntity("tile");
		auto& anim = e.addComponent<CAnimation>(m_game->assets().getAnimation("Block"), true);
		e.addComponent<CTransform>();
		e.addComponent<CBoundingBox>(anim.animation.getSize());
		e.addComponent<CClickable>();
		e.addComponent<CGuiTemplate>();
	}
	{
		// tektite
		auto e = m_entityManager.addEntity("npc");
		auto& anim = e.addComponent<CAnimation>(m_game->assets().getAnimation("Tektite"), true);
		e.addComponent<CTransform>();
		e.addComponent<CBoundingBox>(anim.animation.getSize());
		e.addComponent<CClickable>();
		e.addComponent<CGuiTemplate>();
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
	if (!file.is_open())
	{
		std::cerr << "Could not load from file: " << path << std::endl;
		return;
	}

	std::string tag;
	while (file >> tag)
	{
		if (tag == "player")
		{
		}
		else if (tag == "tile")
		{
			std::string animName;
			Vec2 pos;
			int blockMove, blockVision;
			file >> animName >> blockMove >> blockVision >> pos.x >> pos.y;
			
			auto e = m_entityManager.addEntity("tile");
			auto& anim = e.addComponent<CAnimation>(m_game->assets().getAnimation(animName), true);
			e.addComponent<CTransform>(pos);
			e.addComponent<CBoundingBox>(anim.animation.getSize(), blockMove == 1, blockVision == 1);
			e.addComponent<CDraggable>();
			e.addComponent<CClickable>();
		}
		else if (tag == "npc")
		{
			std::string animName;
			Vec2 pos;
			int blockMove, blockVision;
			file >> animName >> blockMove >> blockVision >> pos.x >> pos.y;
			
			// TODO: AI
			auto e = m_entityManager.addEntity("npc");
			auto& anim = e.addComponent<CAnimation>(m_game->assets().getAnimation(animName), true);
			e.addComponent<CTransform>(pos);
			e.addComponent<CBoundingBox>(anim.animation.getSize(), blockMove == 1, blockVision == 1);
			e.addComponent<CDraggable>();
			e.addComponent<CClickable>();
		}
		else
		{
			std::cerr << "Unknown Entity type: " << tag << std::endl;
		}
	}

	std::cout << "Loaded level from file: " << path << std::endl;
}

void Scene_Editor::saveLevel(const std::string& path)
{
	std::ofstream file(path);
	if (!file.is_open())
	{
		std::cerr << "Could not save to file: " << path << std::endl;
		return;
	}

	for (auto e : m_entityManager.getEntities())
	{
		if (e.hasComponent<CGuiTemplate>() || e.tag() == "camera") { continue; }
		
		auto& pos = e.getComponent<CTransform>().pos;
		auto& bbox = e.getComponent<CBoundingBox>();

		file
			<< e.tag() << " "
			<< e.getComponent<CAnimation>().animation.getName() << " "
			<< (bbox.blockMove ? 1 : 0) << " "
			<< (bbox.blockVision ? 1 : 0) << " "
			<< pos.x << " "
			<< pos.y << std::endl;
	}

	std::cout << "Saved level to file: " << path << std::endl;
}

void Scene_Editor::sDoAction(const Action& action)
{
	bool holdingShift = m_camera.getComponent<CInput>().modifier2;

	if (action.type() == "START")
	{
		if (action.name() == "QUIT" && holdingShift) { onEnd(); }
		else if (action.name() == "TOGGLE_TEXTURE") { m_drawTextures = !m_drawTextures; }
		else if (action.name() == "TOGGLE_COLLISION") { m_drawCollision = !m_drawCollision; }
		else if (action.name() == "TOGGLE_GUI") { m_drawGUI = !m_drawGUI; }
		else if (action.name() == "TOGGLE_SELECTED_BLOCK_MOVE")
		{
			m_shouldSelectedBlockMove = !m_shouldSelectedBlockMove;
			m_blockMoveText.setState(m_shouldSelectedBlockMove ? 1 : 0);
		}
		else if (action.name() == "TOGGLE_SELECTED_BLOCK_VISION")
		{
			m_shouldSelectedBlockVision = !m_shouldSelectedBlockVision;
			m_blockVisionText.setState(m_shouldSelectedBlockVision ? 1 : 0);
		}
		else if (action.name() == "UP") { m_camera.getComponent<CInput>().up = true; }
		else if (action.name() == "DOWN") { m_camera.getComponent<CInput>().down = true; }
		else if (action.name() == "LEFT") { m_camera.getComponent<CInput>().left = true; }
		else if (action.name() == "RIGHT") { m_camera.getComponent<CInput>().right = true; }
		else if (action.name() == "SHIFT") { m_camera.getComponent<CInput>().modifier2 = true; }
		else if (action.name() == "SAVE_LEVEL")
		{
			saveLevel(m_levelPath);
		}
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
		else if (action.name() == "SHIFT") { m_camera.getComponent<CInput>().modifier2 = false; }
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
	int initialDraggingCount = m_draggingCount;

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
				if (!colliding)
				{
					// not overlapping, can place down
					drag.dragging = false;
					m_draggingCount--;

					std::cout << "Stopped dragging at position (" << e.getComponent<CTransform>().pos.x << "," << e.getComponent<CTransform>().pos.y << ")" << std::endl;
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
			// update position to mouse
			e.getComponent<CTransform>().pos = m_mousePos;

			// update its visibility and movement blocking
			auto& bbox = e.getComponent<CBoundingBox>();
			bbox.blockMove = m_shouldSelectedBlockMove;
			bbox.blockVision = m_shouldSelectedBlockVision;
		}
	}

	// update mouse pointer if necessary
	if (loadedCursors)
	{
		if (initialDraggingCount == 0 && m_draggingCount > 0)
		{
			m_game->window().setMouseCursor(m_handCursor);
		}
		else if (initialDraggingCount > 0 && m_draggingCount == 0)
		{
			m_game->window().setMouseCursor(m_arrowCursor);
		}
	}
}

void Scene_Editor::sClicking()
{
	for (auto entity : m_entityManager.getEntities())
	{
		if (!entity.hasComponent<CClickable>()) { continue; }
		auto& click = entity.getComponent<CClickable>();

		if (click.leftClicked)
		{
			if (m_draggingCount == 0
				&& entity.hasComponent<CGuiTemplate>())
			{
				// create a placeable copy and start dragging it
				createEntityFromGuiTemplate(entity);
			}
		}
		if (click.rightClicked)
		{
			// delete entity if it is not gui and not being dragged
			if (!entity.hasComponent<CGuiTemplate>()
				&& (!entity.hasComponent<CDraggable>() || !entity.getComponent<CDraggable>().dragging))
			{
				m_entityManager.destroyEntity(entity);
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

	// draw all entities except gui and dragging
	for (auto e : m_entityManager.getEntities())
	{
		if (!e.hasComponent<CGuiTemplate>()
			&& (!e.hasComponent<CDraggable>() || !e.getComponent<CDraggable>().dragging))
		{
			if (m_drawTextures)
				sRenderDrawEntity(e);
			if (m_drawCollision)
				sRenderDrawCollision(e);
		}
	}

	// draw GUI
	if (m_drawGUI)
	{
		sRenderDrawGUI();
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

void Scene_Editor::sRenderDrawGUI()
{
	Vec2 windowSize(static_cast<float>(width()), static_cast<float>(height()));
	Vec2 viewCenter(m_game->window().getView().getCenter().x, m_game->window().getView().getCenter().y);
	Vec2 viewPos = viewCenter - (windowSize / 2.0f); // top left corner

	Vec2 menuPos(viewPos.x + windowSize.x - m_menuSize.x, viewPos.y);

	// draw menu background
	sf::RectangleShape background;
	background.setSize(sf::Vector2f(m_menuSize.x, m_menuSize.y));
	background.setPosition(sf::Vector2f(menuPos.x, menuPos.y));
	background.setFillColor(sf::Color(0, 0, 0, 255));
	m_game->window().draw(background);

	// draw text
	float textHeight = m_menuText.getGlobalBounds().height;

	m_menuText.setPosition(menuPos.x + 5.0f, menuPos.y);
	m_menuText.setFillColor(sf::Color::White);
	m_menuText.setString(m_levelPath);
	m_game->window().draw(m_menuText);

	m_menuText.setPosition(m_menuText.getPosition().x, m_menuText.getPosition().y + textHeight);
	m_menuText.setString("SHIFT+ESC: Go to menu");
	m_game->window().draw(m_menuText);

	m_menuText.setPosition(m_menuText.getPosition().x, m_menuText.getPosition().y + textHeight);
	m_menuText.setString("P: Save level");
	m_game->window().draw(m_menuText);

	m_menuText.setPosition(m_menuText.getPosition().x, m_menuText.getPosition().y + textHeight);
	m_menuText.setString("G: Toggle GUI");
	m_game->window().draw(m_menuText);

	m_menuText.setPosition(m_menuText.getPosition().x, m_menuText.getPosition().y + textHeight);
	m_menuText.setString("T: Toggle textures");
	m_game->window().draw(m_menuText);

	m_menuText.setPosition(m_menuText.getPosition().x, m_menuText.getPosition().y + textHeight);
	m_menuText.setString("C: Toggle collision boxes");
	m_game->window().draw(m_menuText);

	m_blockMoveText.text().setPosition(m_menuText.getPosition().x, m_menuText.getPosition().y + textHeight);
	m_game->window().draw(m_blockMoveText.text());

	m_blockVisionText.text().setPosition(m_blockMoveText.getPositionUnder());
	m_game->window().draw(m_blockVisionText.text());

	// draw gui entities
	float nextTopY = m_blockVisionText.text().getPosition().y + textHeight + 32.0f;

	for (auto e : m_entityManager.getEntities())
	{
		if (!e.hasComponent<CGuiTemplate>()) { continue; }

		auto& transform = e.getComponent<CTransform>();
		auto& animation = e.getComponent<CAnimation>().animation;
		auto& bbox = e.getComponent<CBoundingBox>();

		// move into menu
		transform.pos.x = menuPos.x + m_menuSize.x / 2.0f;
		transform.pos.y = nextTopY + animation.getSize().y / 2.0f;

		nextTopY += animation.getSize().y + 32.0f;

		// update its visibility and movement blocking
		bbox.blockMove = m_shouldSelectedBlockMove;
		bbox.blockVision = m_shouldSelectedBlockVision;

		// draw regardless of the texture/collision toggles
		sRenderDrawEntity(e);
		sRenderDrawCollision(e);
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

Entity Scene_Editor::createEntityFromGuiTemplate(Entity templ)
{
	auto& anim = templ.getComponent<CAnimation>().animation;
	auto e = m_entityManager.addEntity(templ.tag());
	e.addComponent<CAnimation>(anim, true);
	e.addComponent<CTransform>(m_mousePos);
	e.addComponent<CBoundingBox>(anim.getSize()); // will set properties when placing
	// we want it to be dragged
	// instead of setting flag directly, simulate a left mouse click
	e.addComponent<CDraggable>();
	e.addComponent<CClickable>(true, false);
	// TODO: NPC AI

	return e;
}