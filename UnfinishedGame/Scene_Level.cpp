#include "Scene_Level.h"

Scene_Level::Scene_Level(GameEngine* gameEngine, const std::string& levelPath)
	: Scene(gameEngine)
{
	init(levelPath);
}

void Scene_Level::init(const std::string& levelPath)
{
	registerAction(sf::Keyboard::Escape, "QUIT");
	registerAction(sf::Keyboard::P, "PAUSE");
	registerAction(sf::Keyboard::T, "TOGGLE_TEXTURE");
	registerAction(sf::Keyboard::C, "TOGGLE_COLLISION");
	registerAction(sf::Keyboard::O, "TOGGLE_PARTICLES");
	registerAction(sf::Keyboard::L, "TOGGLE_VISIBILITY");
	registerAction(sf::Keyboard::W, "UP");
	registerAction(sf::Keyboard::S, "DOWN");
	registerAction(sf::Keyboard::A, "LEFT");
	registerAction(sf::Keyboard::D, "RIGHT");

	m_particleSystem.init(width(), height());

	m_mousePos = Vec2(0.0f, 0.0f);

	loadLevel(levelPath);
}

void Scene_Level::update()
{
	m_entityManager.update();

	if (m_paused)
	{

	}
	else
	{
		sMovement();
		sAnimation();
		sCamera();
		m_particleSystem.update();
	}

	m_currentFrame++;
}

void Scene_Level::onEnd()
{
	m_game->changeScene("MENU", nullptr, true);
}

void Scene_Level::loadLevel(const std::string& path)
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

void Scene_Level::sDoAction(const Action& action)
{
	if (action.type() == "START")
	{
		if (action.name() == "PAUSE") { setPaused(!m_paused); }
		else if (action.name() == "QUIT") { onEnd(); }
		else if (action.name() == "TOGGLE_TEXTURE") { m_drawTextures = !m_drawTextures; }
		else if (action.name() == "TOGGLE_COLLISION") { m_drawCollision = !m_drawCollision; }
		else if (action.name() == "TOGGLE_PARTICLES") { m_drawParticles = !m_drawParticles; }
		else if (action.name() == "UP") { m_camera.getComponent<CInput>().up = true; }
		else if (action.name() == "DOWN") { m_camera.getComponent<CInput>().down = true; }
		else if (action.name() == "LEFT") { m_camera.getComponent<CInput>().left = true; }
		else if (action.name() == "RIGHT") { m_camera.getComponent<CInput>().right = true; }
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

void Scene_Level::sMovement()
{
	// move camera according to input
	{
		auto& transform = m_camera.getComponent<CTransform>();
		auto& input = m_camera.getComponent<CInput>();

		transform.velocity = Vec2(0.0f, 0.0f);
		if (input.left) transform.velocity.x -= m_cameraSpeed;
		if (input.right) transform.velocity.x += m_cameraSpeed;
		if (input.up) transform.velocity.y -= m_cameraSpeed;
		if (input.down) transform.velocity.y += m_cameraSpeed;
	}

	// move all entities
	for (auto e : m_entityManager.getEntities())
	{
		if (!e.hasComponent<CTransform>()) { continue; }
		auto& transform = e.getComponent<CTransform>();
		transform.prevPos = transform.pos;
		transform.pos += transform.velocity;
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

void Scene_Level::sCamera()
{
	auto& transform = m_camera.getComponent<CTransform>();

	// set view to be centered on camera
	sf::View view = m_game->window().getView();
	view.setCenter(transform.pos.x, transform.pos.y);
	m_game->window().setView(view);

	// make mouse position correct again
	m_mousePos += transform.pos - transform.prevPos;
}

void Scene_Level::sRender()
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

	if (m_drawParticles)
	{
		m_particleSystem.draw(m_game->window());
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

	{
		sf::CircleShape circle(10.0, 30);
		circle.setOrigin(circle.getRadius(), circle.getRadius());
		circle.setFillColor(sf::Color::Red);
		circle.setPosition(m_mousePos.x, m_mousePos.y);
		m_game->window().draw(circle);
	}
}