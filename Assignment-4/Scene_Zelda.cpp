#include "Scene_Zelda.h"
#include "Physics.h"

int random(const int a, const int b);

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
	registerAction(sf::Keyboard::G, "TOGGLE_GRID");
	registerAction(sf::Keyboard::Y, "TOGGLE_FOLLOW");
	registerAction(sf::Keyboard::A, "LEFT");
	registerAction(sf::Keyboard::D, "RIGHT");
	registerAction(sf::Keyboard::W, "UP");
	registerAction(sf::Keyboard::S, "DOWN");
	registerAction(sf::Keyboard::Space, "ATTACK");

	m_gridText.setFont(m_game->assets().getFont("Arial"));
	m_gridText.setCharacterSize(12);
	
	m_game->playSound("MusicLevel");

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
			entity.addComponent<CAnimation>(m_game->assets().getAnimation(str), true);
			entity.addComponent<CTransform>(getPosition(room, tile, entity));
			entity.addComponent<CBoundingBox>(entity.getComponent<CAnimation>().animation.getSize(),
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
			entity.addComponent<CAnimation>(m_game->assets().getAnimation(str), true);
			entity.addComponent<CTransform>(getPosition(room, tile, entity));
			entity.addComponent<CBoundingBox>(entity.getComponent<CAnimation>().animation.getSize(),
				blockMove == 1, blockVision == 1);
			entity.addComponent<CHealth>(health);
			entity.addComponent<CDamage>(damage);

			if (ai == "Follow")
			{
				float speed; // calc vec to go toward player, normalize, multiply by speed
				file >> speed;
				entity.addComponent<CFollowPlayer>(getPosition(room, tile, entity), speed);
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
				entity.addComponent<CPatrol>(positions, speed);
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

	if (m_paused)
	{

	}
	else
	{
		sAI();
		sMovement();
		sStatus();
		sAttack();
		sCollision();
		sAnimation();
		sCamera();
	}
	
	m_currentFrame++;
}

void Scene_Zelda::onEnd()
{
	// stop the music
	// play the menu music
	// change scene to menu
	m_game->stopSound("MusicLevel");
	m_game->playSound("MusicTitle");
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
		else if (action.name() == "TOGGLE_GRID") { m_drawGrid = !m_drawGrid; }
		else if (action.name() == "TOGGLE_FOLLOW") { m_followCamera = !m_followCamera; }
		else if (action.name() == "LEFT") { m_player.getComponent<CInput>().left = true; }
		else if (action.name() == "RIGHT") { m_player.getComponent<CInput>().right = true; }
		else if (action.name() == "UP") { m_player.getComponent<CInput>().up = true; }
		else if (action.name() == "DOWN") { m_player.getComponent<CInput>().down = true; }
		else if (action.name() == "ATTACK") { m_player.getComponent<CInput>().attack = true; }
	}
	else if (action.type() == "END")
	{
		if (action.name() == "LEFT") { m_player.getComponent<CInput>().left = false; }
		else if (action.name() == "RIGHT") { m_player.getComponent<CInput>().right = false; }
		else if (action.name() == "UP") { m_player.getComponent<CInput>().up = false; }
		else if (action.name() == "DOWN") { m_player.getComponent<CInput>().down = false; }
		else if (action.name() == "ATTACK")
		{
			m_player.getComponent<CInput>().attack = false;
			m_player.getComponent<CInput>().canAttack = true;
		}
	}
}

void Scene_Zelda::sMovement()
{
	// player movement
	{
		auto& input = m_player.getComponent<CInput>();
		auto& transform = m_player.getComponent<CTransform>();

		transform.velocity = Vec2();
		if ((input.left && input.right) || (input.up && input.down))
		{
			transform.facing.x = 0.0f;
			transform.facing.y = 0.0f;
			transform.scale.x = 1.0f;
			m_player.getComponent<CState>().state = "stand";
		}
		else if (input.left)
		{
			transform.velocity.x -= m_playerConfig.SPEED;
			transform.facing.x = -1.0f;
			transform.facing.y = 0.0f;
			transform.scale.x = -1.0f;
			m_player.getComponent<CState>().state = "run";
		}
		else if (input.right)
		{
			transform.velocity.x += m_playerConfig.SPEED;
			transform.facing.x = 1.0f;
			transform.facing.y = 0.0f;
			transform.scale.x = 1.0f;
			m_player.getComponent<CState>().state = "run";

		}
		else if (input.up)
		{
			transform.velocity.y -= m_playerConfig.SPEED;
			transform.facing.x = 0.0f;
			transform.facing.y = -1.0f;
			transform.scale.x = 1.0f;
			m_player.getComponent<CState>().state = "run";
		}
		else if (input.down)
		{
			transform.velocity.y += m_playerConfig.SPEED;
			transform.facing.x = 0.0f;
			transform.facing.y = 1.0f;
			transform.scale.x = 1.0f;
			m_player.getComponent<CState>().state = "run";
		}
		else
		{
			// no walk input
			m_player.getComponent<CState>().state = "stand";
		}

		transform.pos += transform.velocity;
	}
	
	// sword movement
	for (auto s : m_entityManager.getEntities("sword"))
	{
		auto& ptransform = m_player.getComponent<CTransform>();
		s.getComponent<CTransform>().pos = ptransform.pos + ptransform.facing * m_tileSize;
	}

	// npc movement
	// velocities are calculated in the AI system
	for (auto npc : m_entityManager.getEntities("npc"))
	{
		npc.getComponent<CTransform>().pos += npc.getComponent<CTransform>().velocity;
	}
}

void Scene_Zelda::sCollision()
{
	// small optimization
	std::vector<Entity> blackTiles;
	std::vector<Entity> heartTiles;

	for (auto tile : m_entityManager.getEntities("tile"))
	{
		if (tile.getComponent<CAnimation>().animation.getName() == "Black")
			blackTiles.push_back(tile);
		else if (tile.getComponent<CAnimation>().animation.getName() == "Heart")
			heartTiles.push_back(tile);
	}

	// entity-tile collisions
	for (auto entity : m_entityManager.getEntities())
	{
		if (entity.tag() != "player" && entity.tag() != "npc") { continue; }
		if (!entity.hasComponent<CBoundingBox>()) { continue; }
		auto& transform = entity.getComponent<CTransform>();

		for (auto tile : m_entityManager.getEntities("tile"))
		{
			if (!tile.getComponent<CBoundingBox>().blockMove) { continue; }

			Vec2 overlap = Physics::GetOverlap(entity, tile);
			if (!(overlap.x > 0.0f && overlap.y > 0.0f))
			{
				// no overlap
				continue;
			}

			//std::cout << "entity " << entity->getComponent<CAnimation>().animation.getName() << " collided with " << tile->getComponent<CAnimation>().animation.getName() << std::endl;

			Vec2 prevOverlap = Physics::GetPreviousOverlap(entity, tile);

			bool pushbackX = false;
			bool pushbackY = false;
			if (prevOverlap.x > 0.0f && prevOverlap.y > 0.0f)
			{
				pushbackX = true;
				pushbackY = true;
			}
			else if (prevOverlap.y > 0.0f)
				pushbackX = true;
			else if (prevOverlap.x > 0.0f)
				pushbackY = true;
			else
			{
				if (overlap.x > overlap.y)
					pushbackY = true;
				else
					pushbackX = true;
			}

			if (pushbackX)
			{
				// subtract overlap if entity is to the left of the tile, add it otherwise
				bool toLeft = transform.pos.x < tile.getComponent<CTransform>().pos.x;
				transform.pos.x += toLeft ? -overlap.x : overlap.x;
			}
			if (pushbackY)
			{
				// subtract overlap if entity is above the tile, add it otherwise
				bool isEntityBelow = transform.pos.y > tile.getComponent<CTransform>().pos.y;
				transform.pos.y += isEntityBelow ? overlap.y : -overlap.y;
			}
		}
	}

	// player-NPC collisions with appropriate damage calculations
	if (!m_player.hasComponent<CInvincibility>())
	{
		for (auto npc : m_entityManager.getEntities("npc"))
		{
			if (!npc.hasComponent<CDamage>()) { continue; }
			Vec2 overlap = Physics::GetOverlap(m_player, npc);
			if (!(overlap.x > 0.0f && overlap.y > 0.0f)) { continue; }
			
			m_player.getComponent<CHealth>().current -= npc.getComponent<CDamage>().damage;
			if (m_player.getComponent<CHealth>().current <= 0)
			{
				// player died, respawn
				spawnPlayer();
				m_game->playSound("LinkDie");
				break;
			}
			// didn't die
			m_game->playSound("LinkHurt");
			m_player.addComponent<CInvincibility>(30);
		}
	}

	// sword-NPC collisions
	for (auto sword : m_entityManager.getEntities("sword"))
	{
		if (!sword.hasComponent<CDamage>()) { continue; }
		for (auto npc : m_entityManager.getEntities("npc"))
		{
			Vec2 overlap = Physics::GetOverlap(sword, npc);
			if (!(overlap.x > 0.0f && overlap.y > 0.0f)) { continue; }
			
			npc.getComponent<CHealth>().current -= sword.getComponent<CDamage>().damage;
			if (npc.getComponent<CHealth>().current <= 0)
			{
				// enemy died
				m_game->playSound("EnemyDie");
				m_entityManager.destroyEntity(npc);
			}
			else
			{
				// enemy still alive
				m_game->playSound("EnemyHit");
			}

			sword.removeComponent<CDamage>();
			break;
		}
	}

	// black tile collisions / "teleporting"
	if (blackTiles.size() > 1)
	{
		for (auto tile : blackTiles)
		{
			Vec2 overlap = Physics::GetOverlap(m_player, tile);
			if (!(overlap.x > 0.0f && overlap.y > 0.0f)) { continue; }

			// colliding, teleport to another black tile
			int index;
			do
			{
				index = random(0, static_cast<int>(blackTiles.size() - 1));
			} while (blackTiles[index] == tile);

			m_player.getComponent<CTransform>().pos = blackTiles[index].getComponent<CTransform>().pos + Vec2(0.0f, m_tileSize.y);
			// prevent many teleports
			m_player.getComponent<CInput>().up = false;
			break;
		}
	}

	// entity - heart collisions and life gain logic
	for (auto entity : m_entityManager.getEntities())
	{
		if (entity.tag() == "tile") { continue; }
		if (!entity.hasComponent<CBoundingBox>() || !entity.getComponent<CBoundingBox>().blockMove) { continue; }
		if (!entity.hasComponent<CHealth>()) { continue; }

		for (auto heart : heartTiles)
		{
			if (!heart.isActive()) { continue; }
			Vec2 overlap = Physics::GetOverlap(entity, heart);
			if (!(overlap.x > 0.0f && overlap.y > 0.0f)) { continue; }

			entity.getComponent<CHealth>().current = entity.getComponent<CHealth>().max;
			m_entityManager.destroyEntity(heart);
			m_game->playSound("GetItem");
			break;
		}
	}
}

void Scene_Zelda::sAI()
{
	for (auto npc : m_entityManager.getEntities())
	{
		if (npc.hasComponent<CFollowPlayer>())
		{
			auto& follow = npc.getComponent<CFollowPlayer>();
			auto& transform = npc.getComponent<CTransform>();
			
			bool canSeePlayer = true;
			for (auto e : m_entityManager.getEntities())
			{
				if (e.id() == npc.id() || !e.hasComponent<CBoundingBox>() || !e.getComponent<CBoundingBox>().blockVision) { continue; }
				// e is a different entity with a bounding box that blocks vision
				if (Physics::EntityIntersect(transform.pos, m_player.getComponent<CTransform>().pos, e))
				{
					// e blocks vision from npc to player
					canSeePlayer = false;
					break;
				}
			}

			Vec2 desired;
			if (canSeePlayer)
				desired = m_player.getComponent<CTransform>().pos - transform.pos;
			else
				desired = follow.home - transform.pos;

			if (desired.length() <= follow.speed)
			{
				// right next to the target
				// essentially set the position exactly to the target
				// this prevents oscillation
				transform.velocity = desired;
			}
			else
			{
				// far away from the target
				desired = desired.normalize();
				desired *= follow.speed;
				Vec2 steering = desired - transform.velocity;
				steering *= 0.88f;
				transform.velocity += steering;
			}
		}

		if (npc.hasComponent<CPatrol>())
		{
			auto& patrol = npc.getComponent<CPatrol>();
			auto& transform = npc.getComponent<CTransform>();
			if (transform.pos.dist(patrol.positions[patrol.currentPosition]) <= 5.0f)
			{
				// arrived, go to next one
				patrol.currentPosition = (patrol.currentPosition + 1) % patrol.positions.size();
			}

			// move to patrol position
			Vec2 desired = patrol.positions[patrol.currentPosition] - transform.pos;
			desired = desired.normalize();
			desired *= patrol.speed;
			Vec2 steering = desired - transform.velocity;
			steering *= 0.88f;
			transform.velocity += steering;
		}
	}
}

void Scene_Zelda::sAttack()
{
	if (!m_player.isActive()) { return; }

	if (m_player.getComponent<CInput>().attack
		&& m_player.getComponent<CInput>().canAttack
		&& m_entityManager.getEntities("sword").size() == 0)
	{
		// pressing the attack button, not attacking right now
		spawnSword(m_player);
		m_player.getComponent<CState>().state = "attack";
		m_player.getComponent<CInput>().canAttack = false;
	}
}

void Scene_Zelda::sStatus()
{
	for (auto e : m_entityManager.getEntities())
	{
		// implement lifespan
		if (e.hasComponent<CLifespan>())
		{
			auto& lifespan = e.getComponent<CLifespan>();
			if (lifespan.frameCreated + lifespan.lifespan < m_currentFrame)
			{
				m_entityManager.destroyEntity(e);
			}
		}

		// implement invincibility frames
		if (e.hasComponent<CInvincibility>())
		{
			auto& inv = e.getComponent<CInvincibility>();
			inv.iframes -= 1;
			if (inv.iframes <= 0)
			{
				e.removeComponent<CInvincibility>();
			}
		}
	}
}

void Scene_Zelda::sAnimation()
{
	if (m_entityManager.getEntities("sword").size() > 0)
	{
		m_player.getComponent<CState>().state = "attack";
	}

	// player animation
	{
		auto& facing = m_player.getComponent<CTransform>().facing;
		auto& state = m_player.getComponent<CState>().state;
		auto& animName = m_player.getComponent<CAnimation>().animation.getName();
		if (state == "stand")
		{
			if (facing.y == -1.0f && animName != "StandUp")
				m_player.addComponent<CAnimation>(m_game->assets().getAnimation("StandUp"), true);
			else if (facing.y == 1.0f && animName != "StandDown")
				m_player.addComponent<CAnimation>(m_game->assets().getAnimation("StandDown"), true);
			else if (facing.x != 0.0f && animName != "StandRight")
				m_player.addComponent<CAnimation>(m_game->assets().getAnimation("StandRight"), true);
		}
		else if (state == "run")
		{
			if (facing.y == -1.0f && animName != "RunUp")
				m_player.addComponent<CAnimation>(m_game->assets().getAnimation("RunUp"), true);
			else if (facing.y == 1.0f && animName != "RunDown")
				m_player.addComponent<CAnimation>(m_game->assets().getAnimation("RunDown"), true);
			else if (facing.x != 0.0f && animName != "RunRight")
				m_player.addComponent<CAnimation>(m_game->assets().getAnimation("RunRight"), true);
		}
		else if (state == "attack")
		{
			if (facing.y == -1.0f && animName != "AtkUp")
				m_player.addComponent<CAnimation>(m_game->assets().getAnimation("AtkUp"), true);
			else if (facing.y == 1.0f && animName != "AtkDown")
				m_player.addComponent<CAnimation>(m_game->assets().getAnimation("AtkDown"), true);
			else if (facing.x != 0.0f && animName != "AtkRight")
				m_player.addComponent<CAnimation>(m_game->assets().getAnimation("AtkRight"), true);
		}
	}
	
	// sword animation based on player facing
	// (sword should move if player changes direction mid swing)
	for (auto s : m_entityManager.getEntities("sword"))
	{
		auto& ptransform = m_player.getComponent<CTransform>();
		auto& stransform = s.getComponent<CTransform>();
		auto& animName = s.getComponent<CAnimation>().animation.getName();
		if (ptransform.facing.x != 0.0f && animName != "SwordRight")
			s.addComponent<CAnimation>(m_game->assets().getAnimation("SwordRight"), true);
		else if (ptransform.facing.y != 0.0f && animName != "SwordUp")
			s.addComponent<CAnimation>(m_game->assets().getAnimation("SwordUp"), true);

		stransform.scale.x = (ptransform.facing.x < 0.0f) ? -1.0f : 1.0f;
		stransform.scale.y = (ptransform.facing.y < 0.0f) ? 1.0f : -1.0f;
	}

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

void Scene_Zelda::sCamera()
{
	sf::View view = m_game->window().getView();

	if (m_followCamera)
	{
		// player follow camera
		auto& pPos = m_player.getComponent<CTransform>().pos;
		view.setCenter(pPos.x, pPos.y);
	}
	else
	{
		// room-based camera
		// top left corner of tile in the world grid
		auto pos = m_player.getComponent<CTransform>().pos
			- m_player.getComponent<CAnimation>().animation.getSize();
		Vec2 room = pos / (m_tileSize * m_roomSize);
		room.x = std::floor(room.x);
		room.y = std::floor(room.y);
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
		// draw entity health bars
		for (auto& e : m_entityManager.getEntities())
		{
			if (!e.hasComponent<CHealth>()) { continue; }
			auto& h = e.getComponent<CHealth>();
			auto& transform = e.getComponent<CTransform>();

			Vec2 size(64.0f, 6.0f);
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
				tick.setPosition(rect.getPosition() + sf::Vector2f(static_cast<float>(i * 64 * 1 / h.max), 0.0f));
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
			if (e.hasComponent<CPatrol>())
			{
				auto& patrol = e.getComponent<CPatrol>().positions;
				for (size_t i = 0; i < patrol.size(); i++)
				{
					dot.setPosition(patrol[i].x, patrol[i].y);
					m_game->window().draw(dot);
				}
			}
			if (e.hasComponent<CFollowPlayer>())
			{
				sf::VertexArray lines(sf::LinesStrip, 2);
				lines[0].position.x = e.getComponent<CTransform>().pos.x;
				lines[0].position.y = e.getComponent<CTransform>().pos.y;
				lines[0].color = sf::Color::Black;
				lines[1].position.x = m_player.getComponent<CTransform>().pos.x;
				lines[1].position.y = m_player.getComponent<CTransform>().pos.y;
				lines[1].color = sf::Color::Black;
				m_game->window().draw(lines);
				dot.setPosition(e.getComponent<CFollowPlayer>().home.x, e.getComponent<CFollowPlayer>().home.y);
				m_game->window().draw(dot);
			}
		}
	}

	if (m_drawGrid)
	{
		int totalRoomSizeX = static_cast<int>(m_roomSize.x * m_tileSize.x);
		int totalRoomSizeY = static_cast<int>(m_roomSize.y * m_tileSize.y);

		sf::Vertex line[2];
		
		float leftX = m_game->window().getView().getCenter().x - (width() / 2.0f);
		float rightX = leftX + width() + m_tileSize.x;
		float nextGridX = leftX - (static_cast<int>(leftX) % totalRoomSizeX);
		float topY = m_game->window().getView().getCenter().y - (height() / 2.0f);
		float bottomY = topY + height() + m_tileSize.y;

		for (float x = nextGridX; x < rightX; x += m_tileSize.x)
		{
			line[0] = sf::Vector2f(x, topY);
			line[1] = sf::Vector2f(x, topY + static_cast<float>(height()));
			m_game->window().draw(line, 2, sf::Lines);
		}

		for (float y = topY; y < bottomY; y += m_tileSize.y)
		{
			line[0] = sf::Vector2f(leftX, y);
			line[1] = sf::Vector2f(rightX, y);
			m_game->window().draw(line, 2, sf::Lines);
			for (float x = nextGridX; x < rightX; x += m_tileSize.x)
			{
				std::string xCell = std::to_string((static_cast<int>(x) % totalRoomSizeX + totalRoomSizeX) % totalRoomSizeX / static_cast<int>(m_tileSize.x));
				std::string yCell = std::to_string((static_cast<int>(y) % totalRoomSizeY + totalRoomSizeY) % totalRoomSizeY / static_cast<int>(m_tileSize.y));
				m_gridText.setString("(" + xCell + "," + yCell + ")");
				m_gridText.setPosition(x + 3, y + 3);
				m_game->window().draw(m_gridText);
			}
		}
	}
}

Vec2 Scene_Zelda::getPosition(const Vec2& room, const Vec2& tile, Entity entity)
{
	auto& animationSize = entity.getComponent<CAnimation>().animation.getSize();
	return Vec2(
		room.x * m_roomSize.x * m_tileSize.x + tile.x * m_tileSize.x + animationSize.x / 2.0f,
		room.y * m_roomSize.y * m_tileSize.y + tile.y * m_tileSize.y + animationSize.y / 2.0f
	);
}

void Scene_Zelda::spawnPlayer()
{
	if (m_player.isActive())
	{
		m_entityManager.destroyEntity(m_player);
	}
	m_player = m_entityManager.addEntity("player");
	m_player.addComponent<CAnimation>(m_game->assets().getAnimation("StandDown"), true);
	m_player.addComponent<CTransform>(Vec2(m_playerConfig.X, m_playerConfig.Y));
	m_player.addComponent<CBoundingBox>(Vec2(m_playerConfig.CW, m_playerConfig.CH), true, false);
	m_player.addComponent<CInput>();
	m_player.getComponent<CInput>().canAttack = true;
	m_player.addComponent<CState>("stand");
	m_player.addComponent<CHealth>(m_playerConfig.HEALTH);
}

void Scene_Zelda::spawnSword(Entity entity)
{
	auto& etransform = entity.getComponent<CTransform>();
	auto s = m_entityManager.addEntity("sword");
	s.addComponent<CAnimation>(m_game->assets().getAnimation("SwordRight"), true);
	s.addComponent<CTransform>(etransform.pos + etransform.facing * m_tileSize);
	s.addComponent<CBoundingBox>(s.getComponent<CAnimation>().animation.getSize(), false, false);
	s.addComponent<CLifespan>(10, m_currentFrame);
	s.addComponent<CDamage>(1);
	m_game->playSound("Slash");
}

// get number in range [a, b]
int random(const int a, const int b)
{
	return a + (rand() % (1 + b - a));
}