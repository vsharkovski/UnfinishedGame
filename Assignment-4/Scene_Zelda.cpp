#include "Scene_Zelda.h"
#include "Physics.h"

Scene_Zelda::Scene_Zelda(GameEngine* gameEngine, const std::string& levelPath)
	: Scene(gameEngine), m_levelPath(levelPath)
{
	init(m_levelPath);
}

void Scene_Zelda::init(const std::string& levelPath)
{
	registerAction(sf::Keyboard::Escape, "QUIT");
	registerAction(sf::Keyboard::P, "PAUSE");
	registerAction(sf::Keyboard::T, "TOGGLE_TEXTURE");
	registerAction(sf::Keyboard::C, "TOGGLE_COLLISION");
	registerAction(sf::Keyboard::Y, "TOGGLE_FOLLOW");

	loadLevel(levelPath);
}

void Scene_Zelda::loadLevel(const std::string& filename)
{
	m_entityManager = EntityManager();

	std::ifstream file(filename);
	std::string str;
	while (file.good())
	{
		file >> str;
		if (str == "Player")
		{
			file >> m_playerConfig.X >> m_playerConfig.Y;
			file >> m_playerConfig.CW >> m_playerConfig.CH;
			file >> m_playerConfig.SPEED >> m_playerConfig.HEALTH;
		}
		else if (str == "Tile")
		{
			Vec2 room, tile;
			int blockMove, blockVision;
			file >> str >> room.x >> room.y >> tile.x >> tile.y >> blockMove >> blockVision;
			auto entity = m_entityManager.addEntity("tile");
			entity->addComponent<CAnimation>(m_game->assets().getAnimation(str), true);
			entity->addComponent<CTransform>(getPosition(room, tile, entity));
			entity->addComponent<CBoundingBox>(entity->getComponent<CAnimation>().animation.getSize(),
				blockMove == 1, blockVision == 1);
		}
		else if (str == "NPC")
		{
			Vec2 room, tile;
			int blockMove, blockVision;
			int health, damage;
			std::string ai;
			file >> str >> room.x >> room.y >> tile.x >> tile.y;
			file >> blockMove >> blockVision >> health >> damage >> ai;

			auto entity = m_entityManager.addEntity("npc");
			entity->addComponent<CAnimation>(m_game->assets().getAnimation(str), true);
			entity->addComponent<CTransform>(getPosition(room, tile, entity));
			entity->addComponent<CBoundingBox>(entity->getComponent<CAnimation>().animation.getSize(),
				blockMove == 1, blockVision == 1);
			
			if (ai == "Follow")
			{
				float speed; // calc vec to go toward player, normalize, multiply by speed
				file >> speed;
				entity->addComponent<CFollowPlayer>(getPosition(room, tile, entity), speed);
			}
			else if (ai == "Patrol")
			{
				float speed;
				size_t numPositions;
				file >> speed >> numPositions;
				std::vector<Vec2> positions(numPositions);
				for (Vec2& pos : positions)
				{
					file >> tile.x >> tile.y;
					pos = getPosition(room, tile, entity);
				}
				entity->addComponent<CPatrol>(positions, speed);
			}
		}
		else
		{
			std::cerr << "Unknown Entity type: " << str << std::endl;
		}
	}

	spawnPlayer();
}

void Scene_Zelda::update()
{
	m_entityManager.update();

	// todo: implement pause

	sAI();
	sMovement();
	sStatus();
	sCollision();
	sAnimation();
	sCamera();

	m_currentFrame++;
}

void Scene_Zelda::onEnd()
{
	// stop the music
	// play the menu music
	// change scene to menu
	m_game->changeScene("MENU", nullptr, true);
}


void Scene_Zelda::sDoAction(const Action& action)
{
	// do minimal logic in this function
	if (action.type() == "START")
	{
		if (action.name() == "PAUSE") { setPaused(!m_paused); }
		else if (action.name() == "QUIT") { onEnd(); }
		else if (action.name() == "TOGGLE_TEXTURE") { m_drawTextures = !m_drawTextures; }
		else if (action.name() == "TOGGLE_COLLISION") { m_drawCollision = !m_drawCollision; }
		else if (action.name() == "TOGGLE_FOLLOW") { m_followCamera = !m_followCamera; }

	}
	else if (action.type() == "END")
	{

	}
	else if (action.name() == "MOUSE_MOVE")
	{
		// difference in x value between the window's POV and world's POV
		float xDiff = m_game->window().getView().getCenter().x - static_cast<float>(width()) / 2.0f;

	}
}

void Scene_Zelda::sMovement()
{
	// implement all player movement functionality here based on CInput
}

void Scene_Zelda::sCollision()
{
	// entity-tile collisions
	for (auto entity : m_entityManager.getEntities())
	{
		if (!entity->hasComponent<CBoundingBox>()) { continue; }
		auto& transform = entity->getComponent<CTransform>();

		for (auto tile : m_entityManager.getEntities("tile"))
		{
			Vec2 overlap = Physics::GetOverlap(entity, tile);
			if (!(overlap.x > 0.0f && overlap.y > 0.0f))
			{
				// no overlap
				continue;
			}

			if (tile->getComponent<CAnimation>().animation.getName() == "Black" && entity->tag() == "player")
			{
				// teleport the player to another random black tile

			}

			Vec2 prevOverlap = Physics::GetPreviousOverlap(entity, tile);

			bool pushbackX = false;
			bool pushbackY = false;
			if (prevOverlap.x > 0.0f && prevOverlap.y > 0.0f)
			{
				pushbackX = true;
				pushbackY = true;
			}
			else if (prevOverlap.y > 0.0f)
			{
				pushbackX = true;
			}
			else if (prevOverlap.x > 0.0f)
			{
				pushbackY = true;
			}
			else
			{
				if (overlap.x > overlap.y)
				{
					pushbackY = true;
				}
				else {
					pushbackX = true;
				}
			}

			if (pushbackX)
			{
				// subtract overlap if player is to the left of the tile, add it otherwise
				bool toLeft = transform.pos.x < tile->getComponent<CTransform>().pos.x;
				transform.pos.x += toLeft ? -overlap.x : overlap.x;
			}
			if (pushbackY)
			{
				// subtract overlap if player is above the tile, add it otherwise
				bool isEntityBelow = transform.pos.y > tile->getComponent<CTransform>().pos.y;
				transform.pos.y += isEntityBelow ? overlap.y : -overlap.y;
			}
		}
	}

	// player-enemy collisions with appropriate damage calculations

	// sword-NPC collisions

	// black tile collisions / "teleporting"

	// entity - heart collisions and life gain logic

	// use helper functions???
}

void Scene_Zelda::sAI()
{
	// implement follow
	// implement patrol
}

void Scene_Zelda::sStatus()
{
	// implement lifespan
	// implement invincibility frames
}

void Scene_Zelda::sAnimation()
{
	// player facing direction animation
	// use CTransform.facing

	// sword animation based on player facing
	// (sword should move if player changes direction mid swing)


	// destruct entities with non-repeating finished animations
	for (auto e : m_entityManager.getEntities())
	{
		if (!e->hasComponent<CAnimation>()) { continue; }
		auto& animation = e->getComponent<CAnimation>();
		animation.animation.update();
		if (!animation.repeat && animation.animation.hasEnded())
		{
			e->destroy();
		}
	}
}

void Scene_Zelda::sCamera()
{
	sf::View view = m_game->window().getView();

	if (m_followCamera)
	{
		// player follow camera
		auto& pPos = m_player->getComponent<CTransform>().pos;
		Vec2 viewCenter(
			std::max(static_cast<float>(width()) / 2.0f, pPos.x),
			std::max(static_cast<float>(height()) / 2.0f, pPos.y));
		view.setCenter(viewCenter.x, viewCenter.y);
	}
	else
	{
		// room-based camera
		// top left corner of tile in the world grid
		auto pos = m_player->getComponent<CTransform>().pos
			- m_player->getComponent<CAnimation>().animation.getSize();
		Vec2 room(
			static_cast<int>(pos.x) / static_cast<int>(m_tileSize.x * m_roomSize.x),
			static_cast<int>(pos.y) / static_cast<int>(m_tileSize.y * m_roomSize.y));
		Vec2 viewCenter = room * m_roomSize * m_tileSize + (m_roomSize * m_tileSize / 2.0f);
		view.setCenter(viewCenter.x, viewCenter.y);
	}

	m_game->window().setView(view);
}

void Scene_Zelda::sRender()
{
	m_game->window().clear(sf::Color(255, 192, 122));
	sf::RectangleShape tick({ 1.0f, 6.0f });
	tick.setFillColor(sf::Color::Black);
	
	// draw all entity textures / animations
	if (m_drawTextures)
	{
		// draw entity animations
		for (auto e : m_entityManager.getEntities())
		{
			if (!e->hasComponent<CAnimation>()) { continue; }

			sf::Color c = sf::Color::White;
			if (e->hasComponent<CInvincibility>())
			{
				c = sf::Color(255, 255, 255, 128);
			}

			auto& transform = e->getComponent<CTransform>();
			auto& animation = e->getComponent<CAnimation>().animation;
			animation.getSprite().setRotation(transform.angle);
			animation.getSprite().setPosition(transform.pos.x, transform.pos.y);
			animation.getSprite().setScale(transform.scale.x, transform.scale.y);
			animation.getSprite().setColor(c);
			m_game->window().draw(animation.getSprite());
		}
		// draw entity health bars
		for (auto& e : m_entityManager.getEntities())
		{
			if (!e->hasComponent<CHealth>()) { continue; }
			auto& h = e->getComponent<CHealth>();
			auto& transform = e->getComponent<CTransform>();

			Vec2 size(64.0f, 60.0f);
			sf::RectangleShape rect({ size.x, size.y });
			rect.setPosition(transform.pos.x - 32, transform.pos.y - 48);
			rect.setFillColor(sf::Color(96, 96, 96));
			rect.setOutlineColor(sf::Color::Black);
			rect.setOutlineThickness(2);
			m_game->window().draw(rect);

			float ratio = static_cast<float>(h.current) / h.max;
			size.x *= ratio;
			rect.setSize({ size.x, size.y });
			rect.setFillColor(sf::Color(255, 0, 0));
			rect.setOutlineThickness(0);
			m_game->window().draw(rect);

			for (int i = 0; i < h.max; i++)
			{
				tick.setPosition(rect.getPosition() + sf::Vector2f(i * 64 * 1 / h.max, 0));
				m_game->window().draw(tick);
			}

		}
	}

	// draw all entity collision bounding boxes with a rectangleshape
	if (m_drawCollision)
	{
		sf::CircleShape dot(4);
		dot.setFillColor(sf::Color::Black);
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

				if (box.blockMove && box.blockVision) { rect.setOutlineColor(sf::Color::Black); }
				else if (box.blockMove && !box.blockVision) { rect.setOutlineColor(sf::Color::Blue); }
				else if (!box.blockMove && box.blockVision) { rect.setOutlineColor(sf::Color::Red); }
				else if (!box.blockMove && !box.blockVision) { rect.setOutlineColor(sf::Color::White); }
				rect.setOutlineThickness(1);
				m_game->window().draw(rect);
			}
			if (e->hasComponent<CPatrol>())
			{
				auto& patrol = e->getComponent<CPatrol>().positions;
				for (size_t i = 0; i < patrol.size(); i++)
				{
					dot.setPosition(patrol[i].x, patrol[i].y);
					m_game->window().draw(dot);
				}
			}
			if (e->hasComponent<CFollowPlayer>())
			{
				sf::VertexArray lines(sf::LinesStrip, 2);
				lines[0].position.x = e->getComponent<CTransform>().pos.x;
				lines[0].position.y = e->getComponent<CTransform>().pos.y;
				lines[0].color = sf::Color::Black;
				lines[1].position.x = m_player->getComponent<CTransform>().pos.x;
				lines[1].position.y = m_player->getComponent<CTransform>().pos.y;
				lines[1].color = sf::Color::Black;
				m_game->window().draw(lines);
				dot.setPosition(e->getComponent<CFollowPlayer>().home.x, e->getComponent<CFollowPlayer>().home.y);
				m_game->window().draw(dot);
			}
		}
	}
}

Vec2 Scene_Zelda::getPosition(const Vec2& room, const Vec2& tile, std::shared_ptr<Entity> entity)
{
	auto& animationSize = entity->getComponent<CAnimation>().animation.getSize();
	return Vec2(
		room.x * m_roomSize.x + tile.x * m_tileSize.x + animationSize.x / 2.0f,
		room.y * m_roomSize.y + tile.y * m_tileSize.y + animationSize.y / 2.0f
	);
}

void Scene_Zelda::spawnPlayer()
{
	m_player = m_entityManager.addEntity("player");
	m_player->addComponent<CTransform>(Vec2(m_playerConfig.X, m_playerConfig.Y));
	m_player->addComponent<CAnimation>(m_game->assets().getAnimation("StandDown"), true);
	m_player->addComponent<CBoundingBox>(Vec2(m_playerConfig.CW, m_playerConfig.CH), true, false);
	m_player->addComponent<CHealth>(m_playerConfig.HEALTH, m_playerConfig.HEALTH);
}

void Scene_Zelda::spawnSword(std::shared_ptr<Entity> entity)
{
	// give lifespan to sword (10)
	// should spawn at appropriate location based on player's facing direction
	// (position will be player.pos + player.CTransform.facing * tileSize)
	// damage value 1
	// play slash sound
}