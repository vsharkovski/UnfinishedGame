#include "Scene_Play.h"
#include "Common.h"
#include "Physics.h"
#include "Assets.h"
#include "GameEngine.h"
#include "Components.h"
#include "Action.h"

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
	
	m_gridText.setFont(m_game->assets().getFont("Arial"));
	m_gridText.setCharacterSize(12);

	loadLevel(levelPath);
}

Vec2 Scene_Play::gridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity)
{
	// 43:35 on video
	// return Vec2 of where the center of the entity should be
	// use animation size to position it correctly
	// grid height, width: m_gridSize.x, y
	// the bottom left corner of the Animation should align with the bottom left of the grid cell
	return Vec2();
}

void Scene_Play::loadLevel(const std::string& filename)
{
	m_entityManager = EntityManager();
	// read in level file and entities
	// use PlayerConfig struct m_playerConfig to store player properties
	// defined at top of Scene_Play.h

	spawnPlayer();

	auto brick = m_entityManager.addEntity("tile");
	brick->addComponent<CAnimation>(m_game->assets().getAnimation("Brick"), true);
	brick->addComponent<CTransform>(Vec2(96, 480));
	//brick->addComponent<CTransform>(gridToMidPixel(gridX, gridY, brick));

	if (brick->getComponent<CAnimation>().animation.getName() == "Brick")
	{
		// this is a brick!
	}

	auto block = m_entityManager.addEntity("tile");
	block->addComponent<CAnimation>(m_game->assets().getAnimation("Block"), true);
	block->addComponent<CTransform>(Vec2(224, 480));
	block->addComponent<CBoundingBox>(m_game->assets().getAnimation("Block").getSize());

	auto question = m_entityManager.addEntity("tile");
	question->addComponent<CAnimation>(m_game->assets().getAnimation("Question"), true);
	question->addComponent<CTransform>(Vec2(352, 480));

	// components are now returned as references, not pointers
	// must specify reference variable type, otherwise it will copy
	// auto t = e->get<CTransform>();
	// auto& t = e->get<CTransform>();

}

void Scene_Play::spawnPlayer()
{
	m_player = m_entityManager.addEntity("player");
	m_player->addComponent<CAnimation>(m_game->assets().getAnimation("Stand"), true);
	m_player->addComponent<CTransform>(224, 352));
	m_player->addComponent<CBoundingBox>(Vec2(48, 48));
	// add remaining components
}

void Scene_Play::spawnBullet(std::shared_ptr<Entity> entity)
{
	// spawn bullet at entity, going in direction the entity is facing
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
}

void Scene_Play::sMovement()
{
	// implement player movement/jump based on cInput
	// implement gravity
	// implement max player speed in both x and y
	// note: setting animation scale.x to -1/1 will make it flip
}

void Scene_Play::sLifespan()
{
	// check lifespan of entities that have it, and destroy if they go over
}

void Scene_Play::sCollision()
{
	// sfml (0, 0) is at top left
	// jumping has negative y component, gravity has positive
	
	// implement Physics::GetOverlap(), use it here

	// implement bullet, tile collision
	// destroy the tile if it has brick animation

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
		if (action.name() == "TOGGLE_TEXTURE") { m_drawTextures = !m_drawTextures; }
		else if (action.name() == "TOGGLE_COLLISION") { m_drawCollision = !m_drawCollision; }
		else if (action.name() == "TOGGLE_GRID") { m_drawGrid = !m_drawGrid; }
		else if (action.name() == "PAUSE") { setPaused(!m_paused); }
		else if (action.name() == "QUIT") { onEnd(); }
	}
	else if (action.type() == "END")
	{

	}
}

void Scene_Play::sAnimation()
{
	// complete animation class code first
	
	// set animation of player based on cstate component
	
	// for each entity with an animation, call animation.update()
	// if the animation is not repeated, and has ended, destroy the entity
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
	auto& pPos = m_player->getComponent<cTransform>().pos;
	float windowCenterX = std::max(width() / 2.0f, ppos.X);
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
	if (m_drawCollisions)
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
			drawLine(Vec2(x, 0), Vec2(x, height()));
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
}