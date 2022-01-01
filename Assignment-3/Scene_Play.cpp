#include "Scene_Play.h"
#include "Physics.h"
#include "Assets.h"

Scene_Play::Scene_Play(GameEngine* gameEngine, const std::string& levelPath)
	: Scene(gameEngine), m_levelPath(levelPath)
{
	init(m_levelPath);
}

void Scene_Play::init(const std::string& levelPath)
{
	registerAction(sf::Keyboard::Escape, "QUIT");
	registerAction(sf::Keyboard::P, "PAUSE");
	registerAction(sf::Keyboard::T, "TOGGLE_TEXTURE");
	registerAction(sf::Keyboard::C, "TOGGLE_COLLISION");
	registerAction(sf::Keyboard::G, "TOGGLE_GRID");
	registerAction(sf::Keyboard::A, "MOVE_LEFT");
	registerAction(sf::Keyboard::D, "MOVE_RIGHT");
	registerAction(sf::Keyboard::W, "JUMP");
	registerAction(sf::Keyboard::Space, "SHOOT");

	m_mouseShape.setRadius(8.0f);
	m_mouseShape.setOrigin(8.0f, 8.0f);
	m_mouseShape.setPointCount(32);
	m_mouseShape.setFillColor(sf::Color(255, 0, 0, 196));

	m_gridText.setFont(m_game->assets().getFont("Arial"));
	m_gridText.setCharacterSize(12);

	loadLevel(levelPath);
}

void Scene_Play::loadLevel(const std::string& filename)
{
	m_entityManager = EntityManager();

	std::ifstream file(filename);
	std::string str;
	while (file.good())
	{
		file >> str;
		if (str == "Player")
		{
			file >> m_playerConfig.GX >> m_playerConfig.GY;
			file >> m_playerConfig.CW >> m_playerConfig.CH;
			file >> m_playerConfig.SPEED >> m_playerConfig.JUMP >> m_playerConfig.MAXSPEED;
			file >> m_playerConfig.GRAVITY;
			file >> m_playerConfig.WEAPON;
		}
		else if (str == "Tile")
		{
			std::string name;
			float gx, gy;
			file >> name >> gx >> gy;
			auto entity = m_entityManager.addEntity("tile");
			entity->addComponent<CAnimation>(m_game->assets().getAnimation(name), true);
			entity->addComponent<CTransform>(gridToMidPixel(gx, gy, entity));
			entity->addComponent<CBoundingBox>(entity->getComponent<CAnimation>().animation.getSize());
			entity->addComponent<CDraggable>();
		}
		else if (str == "Dec")
		{
			std::string name;
			float gx, gy;
			file >> name >> gx >> gy;
			auto entity = m_entityManager.addEntity("dec");
			entity->addComponent<CAnimation>(m_game->assets().getAnimation(name), true);
			entity->addComponent<CTransform>(gridToMidPixel(gx, gy, entity));
			entity->addComponent<CDraggable>();
		}
		else
		{
			std::cerr << "Unknown Entity type: " << str << std::endl;
		}
	}

	spawnPlayer();
}

void Scene_Play::update()
{
	m_entityManager.update();

	// todo: implement pause

	sMovement();
	sDragging();
	sShoot();
	sLifespan();
	sCollision();
	sAnimation();
	sRender();

	m_currentFrame++;
}

void Scene_Play::onEnd()
{
	// when the scene ends, change back to the menu scene
	m_game->changeScene("MENU", nullptr, true);
}

Vec2 Scene_Play::gridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity)
{
	Vec2 animationSize = entity->getComponent<CAnimation>().animation.getSize();
	return Vec2(
		gridX * m_gridSize.x + animationSize.x / 2.0f,
		static_cast<float>(height()) - gridY * m_gridSize.y - animationSize.y / 2.0f
	);
}

void Scene_Play::spawnPlayer()
{
	m_player = m_entityManager.addEntity("player");
	m_player->addComponent<CAnimation>(m_game->assets().getAnimation("Air"), true);
	m_player->addComponent<CTransform>(gridToMidPixel(m_playerConfig.GX, m_playerConfig.GY, m_player));
	m_player->addComponent<CBoundingBox>(Vec2(m_playerConfig.CW, m_playerConfig.CH));
	m_player->addComponent<CGravity>(m_playerConfig.GRAVITY);
	m_player->addComponent<CInput>();
	m_player->getComponent<CInput>().canShoot = true;
	m_player->addComponent<CState>("JUMP");
	m_player->addComponent<CDraggable>();
}

void Scene_Play::spawnBullet(std::shared_ptr<Entity> entity)
{
	// spawn bullet at entity, going in direction the entity is facing
	auto& transform = entity->getComponent<CTransform>();
	auto bullet = m_entityManager.addEntity("bullet");
	bullet->addComponent<CAnimation>(m_game->assets().getAnimation("Buster"), true);
	bullet->addComponent<CTransform>(
		transform.pos,
		Vec2(2.0f * m_playerConfig.SPEED * transform.scale.x, 0.0f),
		transform.scale,
		transform.angle);
	bullet->addComponent<CBoundingBox>(bullet->getComponent<CAnimation>().animation.getSize());
	bullet->addComponent<CLifespan>(120, m_currentFrame);
}

void Scene_Play::sMovement()
{
	// player movement
	{
		auto& transform = m_player->getComponent<CTransform>();
		auto& input = m_player->getComponent<CInput>();

		transform.prevPos = transform.pos;

		// horizontal movement
		transform.velocity.x = 0.0f;
		if (input.left)		transform.velocity.x -= m_playerConfig.SPEED;
		if (input.right)	transform.velocity.x += m_playerConfig.SPEED;

		//jumping
		if (input.up && input.canJump)
		{
			// starting a jump
			input.canJump = false;
			input.jumping = true;
			
			// apply jumping velocity
			transform.velocity.y += m_playerConfig.JUMP;
		}

		if (input.jumping && !input.up)
		{
			// in the air but not holding up, so stop moving up
			transform.velocity.y = std::max(transform.velocity.y, 0.0f);
			input.jumping = false;
		}

		// gravity
		transform.velocity.y += m_playerConfig.GRAVITY;

		// apply only clamped velocity (each component is in [-maxspeed, maxspeed])
		transform.pos += transform.velocity.clampAbsolute(m_playerConfig.MAXSPEED, m_playerConfig.MAXSPEED);

		if (transform.velocity.x != 0.0f)
		{
			// moved horizontally, so change scale (direction, left/right)
			transform.scale.x = transform.velocity.x < 0.0f ? -1.0f : 1.0f;
		}

		if (!input.canJump)
		{
			// in the air
			m_player->getComponent<CState>().state = "JUMP";
		}
		else
		{
			if (transform.velocity.x == 0.0f)
				m_player->getComponent<CState>().state = "STAND";
			else
				m_player->getComponent<CState>().state = "RUN";
		}

	}

	for (auto& bullet : m_entityManager.getEntities("bullet"))
	{
		auto& transform = bullet->getComponent<CTransform>();
		transform.prevPos = transform.pos;
		transform.pos += transform.velocity;
	}
}

void Scene_Play::sCollision()
{
	for (auto bullet : m_entityManager.getEntities("bullet"))
	{
		for (auto tile : m_entityManager.getEntities("tile"))
		{
			Vec2 overlap = Physics::GetOverlap(bullet, tile);
			if (!(overlap.x > 0.0f && overlap.y > 0.0f))
			{
				// no overlap
				continue;
			}
			if (tile->getComponent<CAnimation>().animation.getName() == "Brick")
			{
				tile->addComponent<CAnimation>(m_game->assets().getAnimation("Explosion"), false);
				tile->removeComponent<CBoundingBox>();
			}
			bullet->destroy();
			break;
		}
	}

	auto& ptransform = m_player->getComponent<CTransform>();
	bool hitAbove = false; // whether the player hit a tile that is above the player
	bool hitBelow = false; // below

	for (auto tile : m_entityManager.getEntities("tile"))
	{
		Vec2 overlap = Physics::GetOverlap(m_player, tile);
		if (!(overlap.x > 0.0f && overlap.y > 0.0f))
		{
			// no overlap
			continue;
		}

		Vec2 prevOverlap = Physics::GetPreviousOverlap(m_player, tile);
		ptransform.prevPos = ptransform.pos;

		bool pushbackX = false;
		bool pushbackY = false;
		if (prevOverlap.y > 0.0f)
			pushbackX = true;
		else if (prevOverlap.x > 0.0f)
			pushbackY = true;
		else
			pushbackY = true;

		if (pushbackX)
		{
			// subtract overlap if player is to the left of the tile, add it otherwise
			bool toLeft = ptransform.pos.x < tile->getComponent<CTransform>().pos.x;
			ptransform.pos.x += toLeft ? -overlap.x : overlap.x;
		}
		if (pushbackY)
		{
			// subtract overlap if player is above the tile, add it otherwise
			bool isPlayerBelow = ptransform.pos.y > tile->getComponent<CTransform>().pos.y;
			ptransform.pos.y += isPlayerBelow ? overlap.y : -overlap.y;
			hitAbove = hitAbove || isPlayerBelow;
			hitBelow = hitBelow || !isPlayerBelow;

			if (isPlayerBelow)
			{
				auto& name = tile->getComponent<CAnimation>().animation.getName();
				if (name == "Brick")
				{
					tile->addComponent<CAnimation>(m_game->assets().getAnimation("Explosion"), false);
					tile->removeComponent<CBoundingBox>();
				}
				else if (name == "Question")
				{

				}
			}
		}
	}

	if (hitBelow)
	{
		// standing, or just landed
		ptransform.velocity.y = 0.0f;
		m_player->getComponent<CInput>().canJump = true;
		m_player->getComponent<CInput>().jumping = false;
	}
	if (hitAbove)
	{
		ptransform.velocity.y = 0.0f;
		// not actively "jumping" anymore
		m_player->getComponent<CInput>().jumping = false;
	}

	// don't let the player walk off the left side of the map
	if (ptransform.pos.x - m_player->getComponent<CBoundingBox>().halfSize.x < 0.0f)
	{
		ptransform.pos.x += -(ptransform.pos.x - m_player->getComponent<CBoundingBox>().halfSize.x);
		ptransform.prevPos = ptransform.pos;
	}

	// check to see if player has fallen down a hole
	if (ptransform.pos.y > static_cast<float>(height()))
	{
		spawnPlayer();
	}
}

void Scene_Play::sLifespan()
{
	// check lifespan of entities that have it, and destroy if they go over
	for (auto entity : m_entityManager.getEntities())
	{
		if (!entity->hasComponent<CLifespan>())
			continue;
		auto& lifespan = entity->getComponent<CLifespan>();
		if (lifespan.frameCreated + lifespan.lifespan < m_currentFrame)
		{
			entity->destroy();
		}
	}
}

void Scene_Play::sShoot()
{
	auto& input = m_player->getComponent<CInput>();
	if (input.canShoot and input.shoot)
	{
		input.canShoot = false;
		spawnBullet(m_player);
	}
}

void Scene_Play::sDoAction(const Action& action)
{
	//std::cout << "Action: " << action.toString() << std::endl;
	if (action.type() == "START")
	{
		if (action.name() == "TOGGLE_TEXTURE")			{ m_drawTextures = !m_drawTextures; }
		else if (action.name() == "TOGGLE_COLLISION")	{ m_drawCollision = !m_drawCollision; }
		else if (action.name() == "TOGGLE_GRID")		{ m_drawGrid = !m_drawGrid; }
		else if (action.name() == "PAUSE")				{ setPaused(!m_paused); }
		else if (action.name() == "QUIT")				{ onEnd(); }
		else if (action.name() == "MOVE_LEFT")			{ m_player->getComponent<CInput>().left		= true; }
		else if (action.name() == "MOVE_RIGHT")			{ m_player->getComponent<CInput>().right	= true; }
		else if (action.name() == "JUMP")				{ m_player->getComponent<CInput>().up		= true; }
		else if (action.name() == "SHOOT")				{ m_player->getComponent<CInput>().shoot	= true; }
		else if (action.name() == "LEFT_CLICK")
		{
			float xDiff = m_game->window().getView().getCenter().x - static_cast<float>(width()) / 2.0f;
			// where we actually clicked inside the world of our game
			Vec2 worldPos(xDiff + action.pos().x, action.pos().y);
			// check to see if the entity was clicked at this position
			for (auto& e : m_entityManager.getEntities())
			{
				if (e->hasComponent<CDraggable>() && Physics::IsInside(worldPos, e))
				{
					e->getComponent<CDraggable>().dragging = !e->getComponent<CDraggable>().dragging;
					std::cout << "Entity clicked: " << e->tag() << std::endl;
					break;
				}
			}
		}
	}
	else if (action.type() == "END")
	{
		if (action.name() == "MOVE_LEFT")		{ m_player->getComponent<CInput>().left		= false; }
		else if (action.name() == "MOVE_RIGHT")	{ m_player->getComponent<CInput>().right	= false; }
		else if (action.name() == "JUMP")		{ m_player->getComponent<CInput>().up		= false; }
		else if (action.name() == "SHOOT")
		{
			m_player->getComponent<CInput>().shoot = false;
			m_player->getComponent<CInput>().canShoot = true;
		}
	}
	else if (action.name() == "MOUSE_MOVE")
	{
		// difference in x value between the window's POV and world's POV
		float xDiff = m_game->window().getView().getCenter().x - static_cast<float>(width()) / 2.0f;
		
		m_mouseShape.setPosition(xDiff + action.pos().x, action.pos().y);
	}
}

void Scene_Play::sAnimation()
{
	// set animation of player based on cstate component
	auto& state = m_player->getComponent<CState>().state;
	auto& animationName = m_player->getComponent<CAnimation>().animation.getName();
	if (state == "STAND" && animationName != "Stand")
	{
		m_player->addComponent<CAnimation>(m_game->assets().getAnimation("Stand"), true);
	}
	else if (state == "RUN" && animationName != "Run")
	{
		m_player->addComponent<CAnimation>(m_game->assets().getAnimation("Run"), true);
	}
	else if (state == "JUMP" && animationName != "Air")
	{
		m_player->addComponent<CAnimation>(m_game->assets().getAnimation("Air"), true);
	}

	// for each entity with an animation, call animation.update()
	for (auto e : m_entityManager.getEntities())
	{
		if (!e->hasComponent<CAnimation>())
			continue;
		auto& animation = e->getComponent<CAnimation>();
		animation.animation.update();
		if (!animation.repeat && animation.animation.hasEnded())
		{
			e->destroy();
		}
	}
}

void Scene_Play::drawLine(const Vec2& p1, const Vec2& p2)
{
	sf::Vertex line[] = { sf::Vector2f(p1.x, p1.y), sf::Vector2f(p2.x, p2.y) };
	m_game->window().draw(line, 2, sf::Lines);
}

void Scene_Play::sRender()
{
	// color the background darker so you know that the game is paused
	if (!m_paused) { m_game->window().clear(sf::Color(100, 100, 255)); }
	else { m_game->window().clear(sf::Color(50, 50, 150)); }

	// set the viewpoint of the window to be centered on the player if it's far enough right
	auto& pPos = m_player->getComponent<CTransform>().pos;
	float windowCenterX = std::max(static_cast<float>(width()) / 2.0f, pPos.x);
	sf::View view = m_game->window().getView();
	view.setCenter(windowCenterX, static_cast<float>(height()) - view.getCenter().y);
	m_game->window().setView(view);

	// draw all entity textures / animations
	if (m_drawTextures)
	{
		for (auto e : m_entityManager.getEntities())
		{
			auto& transform = e->getComponent<CTransform>();
			if (e->hasComponent<CAnimation>())
			{
				auto& animation = e->getComponent<CAnimation>().animation;
				animation.getSprite().setRotation(transform.angle);
				animation.getSprite().setPosition(transform.pos.x, transform.pos.y);
				animation.getSprite().setScale(transform.scale.x, transform.scale.y);
				m_game->window().draw(animation.getSprite());
			}
		}
	}

	// draw all entity collision bounding boxes with a rectangleshape
	if (m_drawCollision)
	{
		for (auto e : m_entityManager.getEntities())
		{
			if (e->hasComponent<CBoundingBox>())
			{
				auto& box = e->getComponent<CBoundingBox>();
				auto& transform = e->getComponent<CTransform>();
				sf::RectangleShape rect;
				rect.setSize(sf::Vector2f(box.size.x - 1, box.size.y - 1));
				rect.setOrigin(sf::Vector2f(box.halfSize.x, box.halfSize.y));
				rect.setPosition(transform.pos.x, transform.pos.y);
				rect.setFillColor(sf::Color(0, 0, 0, 0));
				rect.setOutlineColor(sf::Color(255, 255, 255, 255));
				rect.setOutlineThickness(1);
				m_game->window().draw(rect);
			}
		}
	}

	// draw the grid for easy debugging
	if (m_drawGrid)
	{
		float leftX = m_game->window().getView().getCenter().x - (width() / 2.0f);
		float rightX = leftX + width() + m_gridSize.x;
		float nextGridX = leftX - (static_cast<int>(leftX) % static_cast<int>(m_gridSize.x));
		
		for (float x = nextGridX; x < rightX; x += m_gridSize.x)
		{
			drawLine(Vec2(x, 0), Vec2(x, static_cast<float>(height())));
		}
		
		for (float y = 0; y < height(); y += m_gridSize.y)
		{
			drawLine(Vec2(leftX, height() - y), Vec2(rightX, height() - y));
			for (float x = nextGridX; x < rightX; x += m_gridSize.x)
			{
				std::string xCell = std::to_string(static_cast<int>(x) / static_cast<int>(m_gridSize.x));
				std::string yCell = std::to_string(static_cast<int>(y) / static_cast<int>(m_gridSize.y));
				m_gridText.setString("(" + xCell + "," + yCell + ")");
				m_gridText.setPosition(x + 3, height() - y - m_gridSize.y + 2);
				m_game->window().draw(m_gridText);
			}
		}
	}

	m_game->window().draw(m_mouseShape);

	//std::stringstream ss;
	//ss << std::setprecision(2) << std::fixed;
	//ss << "Player velocity: (" << m_player->getComponent<CTransform>().velocity.x << "," << m_player->getComponent<CTransform>().velocity.y << ")";
	//m_gridText.setString(ss.str());
	//m_gridText.setPosition(10.0f, 10.0f);
	//m_game->window().draw(m_gridText);

	m_game->window().display();
}

void Scene_Play::sDragging()
{
	// update dragging entities
	for (auto e : m_entityManager.getEntities())
	{
		if (e->hasComponent<CDraggable>() && e->getComponent<CDraggable>().dragging)
		{
			// set the position of the entity to the mouse position
			e->getComponent<CTransform>().pos = Vec2(m_mouseShape.getPosition().x, m_mouseShape.getPosition().y);
		}
	}
}