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
	registerAction(sf::Keyboard::G, "TOGGLE_GRID");

	loadLevel(levelPath);
}

void Scene_Zelda::onEnd()
{
	m_game->changeScene("MENU", nullptr, true);
}

Vec2 Scene_Zelda::gridToMidPixel(const Vec2& room, const Vec2& tile, std::shared_ptr<Entity> entity)
{
	Vec2 animationSize = entity->getComponent<CAnimation>().animation.getSize();
	return Vec2(
		room.x * m_roomSize.x + tile.x * m_tileSize.x + animationSize.x / 2.0f,
		room.y * m_roomSize.y + tile.y * m_tileSize.y + animationSize.y / 2.0f
	);
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
			entity->addComponent<CTransform>(gridToMidPixel(room, tile, entity));
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
			entity->addComponent<CTransform>(gridToMidPixel(room, tile, entity));
			entity->addComponent<CBoundingBox>(entity->getComponent<CAnimation>().animation.getSize(),
				blockMove == 1, blockVision == 1);
			
			if (ai == "Follow")
			{
				float speed; // calc vec to go toward player, normalize, multiply by speed
				file >> speed;
				entity->addComponent<CFollowPlayer>(gridToMidPixel(room, tile, entity), speed);
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
					pos = gridToMidPixel(room, tile, entity);
				}
				entity->addComponent<CPatrol>(positions, speed);
			}
		}
		else
		{
			std::cerr << "Unknown Entity type: " << str << std::endl;
		}
	}


}

void Scene_Zelda::update()
{
	m_entityManager.update();


	m_currentFrame++;
}

void Scene_Zelda::sMovement()
{

}

void Scene_Zelda::sCollision()
{
	auto& ptransform = m_player->getComponent<CTransform>();
	for (auto tile : m_entityManager.getEntities("tile"))
	{
		Vec2 overlap = Physics::GetOverlap(m_player, tile);
		if (!(overlap.x > 0.0f && overlap.y > 0.0f))
		{
			// no overlap
			continue;
		}

		if (tile->getComponent<CAnimation>().animation.getName() == "Black")
		{
			// teleport the player to another random black tile

		}

		Vec2 prevOverlap = Physics::GetPreviousOverlap(m_player, tile);

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
			bool toLeft = ptransform.pos.x < tile->getComponent<CTransform>().pos.x;
			ptransform.pos.x += toLeft ? -overlap.x : overlap.x;
		}
		if (pushbackY)
		{
			// subtract overlap if player is above the tile, add it otherwise
			bool isPlayerBelow = ptransform.pos.y > tile->getComponent<CTransform>().pos.y;
			ptransform.pos.y += isPlayerBelow ? overlap.y : -overlap.y;
		}
	}


}

void Scene_Zelda::sAnimation()
{
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

void Scene_Zelda::sDoAction(const Action& action)
{
	if (action.type() == "START")
	{
		if (action.name() == "TOGGLE_TEXTURE") { m_drawTextures = !m_drawTextures; }
		else if (action.name() == "TOGGLE_COLLISION") { m_drawCollision = !m_drawCollision; }
		else if (action.name() == "TOGGLE_GRID") { m_drawGrid = !m_drawGrid; }
		else if (action.name() == "PAUSE") { setPaused(!m_paused); }
		else if (action.name() == "QUIT") { onEnd(); }
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

void Scene_Zelda::sRender()
{

}