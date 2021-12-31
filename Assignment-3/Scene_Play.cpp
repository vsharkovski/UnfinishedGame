#include "Scene_Play.h"
#include "Common.h"
#include "Physics.h"
#include "Assets.h"
#include "Components.h"

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
	registerAction(sf::Keyboard::W, "MOVE_UP");
	registerAction(sf::Keyboard::S, "MOVE_DOWN");
	registerAction(sf::Keyboard::Space, "SHOOT");

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
			entity->addComponent<CAnimation>(m_game->assets().getAnimation("TempBlock"), true);
			entity->addComponent<CTransform>(gridToMidPixel(gx, gy, entity));
			entity->addComponent<CBoundingBox>(entity->getComponent<CAnimation>().animation.getSize());
		}
		else if (str == "Dec")
		{
			std::string name;
			float x, y;
			file >> name >> x >> y;
		}
		else
		{
			std::cerr << "Unknown Entity type: " << str << std::endl;
		}
	}

	spawnPlayer();

	//auto brick = m_entityManager.addEntity("tile");
	//brick->addComponent<CAnimation>(m_game->assets().getAnimation("TempBlock"), true);
	//brick->addComponent<CTransform>(Vec2(96, 480));
	////brick->addComponent<CTransform>(gridToMidPixel(gridX, gridY, brick));

	//if (brick->getComponent<CAnimation>().animation.getName() == "TempBlock")
	//{
	//	// this is a brick!
	//}

	//auto block = m_entityManager.addEntity("tile");
	//block->addComponent<CAnimation>(m_game->assets().getAnimation("TempBlock"), true);
	//block->addComponent<CTransform>(Vec2(224, 480));
	//block->addComponent<CBoundingBox>(m_game->assets().getAnimation("TempBlock").getSize());

	//auto question = m_entityManager.addEntity("tile");
	//question->addComponent<CAnimation>(m_game->assets().getAnimation("TempBlock"), true);
	//question->addComponent<CTransform>(Vec2(352, 480));
}

void Scene_Play::update()
{
	m_entityManager.update();

	// todo: implement pause

	sMovement();
	sLifespan();
	sCollision();
	sAnimation();
	sRender();

	m_currentFrame++;
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
	m_player->addComponent<CAnimation>(m_game->assets().getAnimation("TempPlr"), true);
	m_player->addComponent<CTransform>(gridToMidPixel(m_playerConfig.GX, m_playerConfig.GY, m_player));
	m_player->addComponent<CBoundingBox>(Vec2(m_playerConfig.CW, m_playerConfig.CH));
	m_player->addComponent<CGravity>(m_playerConfig.GRAVITY);
	m_player->addComponent<CInput>();
	m_player->getComponent<CInput>().canShoot = true;
	m_player->getComponent<CInput>().canJump = true;
	// add remaining components
}

void Scene_Play::spawnBullet(std::shared_ptr<Entity> entity)
{
	// spawn bullet at entity, going in direction the entity is facing
	if (!entity->hasComponent<CInput>() || !entity->getComponent<CInput>().canShoot)
	{
		return;
	}
	auto& etransform = entity->getComponent<CTransform>();
	auto bullet = m_entityManager.addEntity("bullet");
	bullet->addComponent<CAnimation>(m_game->assets().getAnimation("TempBullet"), true);
	bullet->addComponent<CTransform>(
		etransform.pos,
		Vec2(m_playerConfig.SPEED * etransform.scale.x, 0.0f),
		etransform.scale,
		etransform.angle);
	bullet->addComponent<CBoundingBox>(bullet->getComponent<CAnimation>().animation.getSize());
	bullet->addComponent<CLifespan>(120, m_currentFrame);
}

void Scene_Play::sMovement()
{
	// player movement
	{
		auto& transform = m_player->getComponent<CTransform>();
		auto& input = m_player->getComponent<CInput>();

		transform.velocity = Vec2();
		if (input.left)		transform.velocity.x -= m_playerConfig.SPEED;
		if (input.right)	transform.velocity.x += m_playerConfig.SPEED;
		if (input.up)		transform.velocity.y -= m_playerConfig.SPEED;
		if (input.down)		transform.velocity.y += m_playerConfig.SPEED;

		// TODO: implement gravity


		transform.velocity.clampAbsolute(m_playerConfig.MAXSPEED, m_playerConfig.MAXSPEED);

		transform.prevPos = transform.pos;
		transform.pos += transform.velocity;

		if (transform.velocity.x != 0.0f)
		{
			// actually moved horizontally, so change scale (direction, left/right)
			transform.scale.x = transform.velocity.x < 0 ? -1.0f : 1.0f;
		}
	}

	for (auto& bullet : m_entityManager.getEntities("bullet"))
	{
		auto& transform = bullet->getComponent<CTransform>();
		transform.prevPos = transform.pos;
		transform.pos += transform.velocity;
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

void Scene_Play::sCollision()
{
	for (auto& bullet : m_entityManager.getEntities("bullet"))
	{
		for (auto& tile : m_entityManager.getEntities("tile"))
		{
			Vec2 overlap = Physics::GetOverlap(bullet, tile);
			if (overlap.x > 0.0f && overlap.y > 0.0f)
			{
				if (tile->getComponent<CAnimation>().animation.getName() == "TempBlock")
				{
					tile->addComponent<CAnimation>(m_game->assets().getAnimation("TempExplosion"), false);
					tile->removeComponent<CBoundingBox>();
				}
				bullet->destroy();
				break;
			}
		}
	}

	// implement player/tile collision and resolutions
	// Update CState component of the player to store whether 
	// it is currently on the ground or in the air. This is 
	// used by animation system

	// check to see if player has fallen down a hole (y > height())
	
	// don't let the player walk off the left side of the map


}

void Scene_Play::sDoAction(const Action& action)
{
	if (action.type() == "START")
	{
		if (action.name() == "TOGGLE_TEXTURE")			{ m_drawTextures = !m_drawTextures; }
		else if (action.name() == "TOGGLE_COLLISION")	{ m_drawCollision = !m_drawCollision; }
		else if (action.name() == "TOGGLE_GRID")		{ m_drawGrid = !m_drawGrid; }
		else if (action.name() == "PAUSE")				{ setPaused(!m_paused); }
		else if (action.name() == "QUIT")				{ onEnd(); }
		else if (action.name() == "MOVE_LEFT")			{ m_player->getComponent<CInput>().left		= true; }
		else if (action.name() == "MOVE_RIGHT")			{ m_player->getComponent<CInput>().right	= true; }
		else if (action.name() == "MOVE_UP")			{ m_player->getComponent<CInput>().up		= true; }
		else if (action.name() == "MOVE_DOWN")			{ m_player->getComponent<CInput>().down		= true; }
		else if (action.name() == "SHOOT")
		{
			spawnBullet(m_player);
			m_player->getComponent<CInput>().canShoot = false;
		}
	}
	else if (action.type() == "END")
	{
		if (action.name() == "MOVE_LEFT")		{ m_player->getComponent<CInput>().left		= false; }
		else if (action.name() == "MOVE_RIGHT")	{ m_player->getComponent<CInput>().right	= false; }
		else if (action.name() == "MOVE_UP")	{ m_player->getComponent<CInput>().up		= false; }
		else if (action.name() == "MOVE_DOWN")	{ m_player->getComponent<CInput>().down		= false; }
		else if (action.name() == "SHOOT")		{ m_player->getComponent<CInput>().canShoot = true; }
	}
}

void Scene_Play::sAnimation()
{
	// note: to set animation to a different one (not update the same one), use setComponent
	// complete animation class code first
	
	// set animation of player based on cstate component
	
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

void Scene_Play::onEnd()
{
	// when the scene ends, change back to the menu scene
	m_game->changeScene("MENU", nullptr, true);
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
	//std::cout << "pPos(" << pPos.x << "," << pPos.y << ")" << std::endl;
	float windowCenterX = std::max(width() / 2.0f, pPos.x);
	sf::View view = m_game->window().getView();
	view.setCenter(windowCenterX, height() - view.getCenter().y);
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

	m_game->window().display();
}