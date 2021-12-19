#include "Game.h"

const float pi = 3.14159265358979f;
const float sqrt2 = sqrt(2.0f);

int random(const int a, const int b);
float random(const float a, const float b);
bool areColliding(const std::shared_ptr<Entity> e1, const std::shared_ptr<Entity> e2);
std::pair<bool, bool> detectBorderCollision(const std::shared_ptr<Entity> e);

Game::Game(const std::string& config)
{
    init(config);
}

void Game::init(const std::string& path)
{
	std::ifstream file(path);
    if (!file.is_open())
    {
        std::cerr << "Could not open config file." << std::endl;
        exit(-1);
    }

    int wWidth = 600;
    int wHeight = 480;
    int frameLimit = 60;
    int isFullScreen = 0;
   
    std::string command;
    while (file >> command)
    {
        if (command == "Window")
        {
            file >> wWidth >> wHeight >> frameLimit >> isFullScreen;
        }
        else if (command == "Font")
        {
            std::string path;
            int fontSize;
            int r, g, b;
            file >> path >> fontSize >> r >> g >> b;
            if (!m_font.loadFromFile(path))
            {
                std::cerr << "Could not load font!" << std::endl;
                exit(-1);
            }
            auto fontColor = sf::Color(r, g, b);
        }
        else if (command == "Player")
        {
            file >> m_playerConfig.SR >> m_playerConfig.CR >> m_playerConfig.S >> m_playerConfig.FR >> m_playerConfig.FG >> m_playerConfig.FB >> m_playerConfig.OR >> m_playerConfig.OG >> m_playerConfig.OB >> m_playerConfig.OT >> m_playerConfig.V;
        }
        else if (command == "Enemy")
        {
            file >> m_enemyConfig.SR >> m_enemyConfig.CR >> m_enemyConfig.SMIN >> m_enemyConfig.SMAX >> m_enemyConfig.OR >> m_enemyConfig.OG >> m_enemyConfig.OB >> m_enemyConfig.OT >> m_enemyConfig.VMIN >> m_enemyConfig.VMAX >> m_enemyConfig.L >> m_enemyConfig.SI;
        }
        else if (command == "Bullet")
        {
            file >> m_bulletConfig.SR >> m_bulletConfig.CR >> m_bulletConfig.S >> m_bulletConfig.FR >> m_bulletConfig.FG >> m_bulletConfig.FB >> m_bulletConfig.OR >> m_bulletConfig.OG >> m_bulletConfig.OB >> m_bulletConfig.OT >> m_bulletConfig.V >> m_bulletConfig.L;
        }
    }

    if (isFullScreen)
        m_window.create(sf::VideoMode(wWidth, wHeight), "Assignment 2", sf::Style::Fullscreen);
    else
        m_window.create(sf::VideoMode(wWidth, wHeight), "Assignment 2", sf::Style::Default);

    m_window.setFramerateLimit(frameLimit);

    spawnPlayer();
}

void Game::run()
{
    // TODO: add pause functionality
    // some systems should function while paused (rendering)
    // some systems shouldn't (movement, input)
    while (m_running)
    {
        if (!m_paused)
        {
            m_entities.update();

            sEnemySpawner();
            sMovement();
            sCollision();
        }

        sUserInput();
        sRender();
            
        m_currentFrame++;
    }
}

void Game::setPaused(bool paused)
{
    m_paused = paused;
}

// spawn the player in the middle of the screen
void Game::spawnPlayer()
{
    auto entity = m_entities.addEntity("player");
    
    sf::Vector2u windowSize = m_window.getSize();
    entity->cTransform = std::make_shared<CTransform>(
        Vec2(static_cast<float>(windowSize.x/2), static_cast<float>(windowSize.y/2)),
        Vec2(0.0f, 0.0f),
        0.0f);
    
    entity->cShape = std::make_shared<CShape>(
        m_playerConfig.SR,
        m_playerConfig.V,
        sf::Color(m_playerConfig.FR, m_playerConfig.FG, m_playerConfig.FB),
        sf::Color(m_playerConfig.OR, m_playerConfig.OG, m_playerConfig.OB),
        m_playerConfig.OT);
    
    entity->cCollision = std::make_shared<CCollision>(m_playerConfig.CR);

    entity->cInput = std::make_shared<CInput>();
    
    m_player = entity;
}

void Game::spawnEnemy()
{
    auto entity = m_entities.addEntity("enemy");
    
    sf::Vector2u windowSize = m_window.getSize();
    float margin = static_cast<float>(m_enemyConfig.SR);
    float spawnX = random(margin, static_cast<float>(windowSize.x) - margin);
    float spawnY = random(margin, static_cast<float>(windowSize.y) - margin);
    float speed = random(m_enemyConfig.SMIN, m_enemyConfig.SMAX);
    float angle = pi * static_cast<float>(random(0, 360 - 1)) / 180.0f; // angle in radians
    entity->cTransform = std::make_shared<CTransform>(
        Vec2(spawnX, spawnY), Vec2(speed, speed), angle);

    int vertices = random(m_enemyConfig.VMIN, m_enemyConfig.VMAX);
    entity->cShape = std::make_shared<CShape>(
        m_enemyConfig.SR,
        vertices,
        sf::Color(0, 0, 0),
        sf::Color(m_enemyConfig.OR, m_enemyConfig.OG, m_enemyConfig.OB),
        m_enemyConfig.OT);

    entity->cCollision = std::make_shared<CCollision>(m_enemyConfig.CR);
    entity->cScore = std::make_shared<CScore>(vertices * 100);

    m_lastEnemySpawnTime = m_currentFrame;
}

void Game::spawnSmallEnemies(const std::shared_ptr<Entity> e)
{
    int vertices = e->cShape->circle.getPointCount();
    float angle = pi * 2.0f / static_cast<float>(vertices);
    for (int i = 0; i < vertices; i++)
    {
        auto entity = m_entities.addEntity("small");
        
        entity->cTransform = std::make_shared<CTransform>(
            e->cTransform->pos, e->cTransform->velocity, angle * i);

        entity->cShape = std::make_shared<CShape>(
            e->cShape->circle.getRadius() / 2.0f,
            vertices,
            e->cShape->circle.getFillColor(),
            e->cShape->circle.getOutlineColor(),
            e->cShape->circle.getOutlineThickness());
        
        entity->cCollision = std::make_shared<CCollision>(e->cCollision->radius / 2.0f);
        entity->cScore = std::make_shared<CScore>(e->cScore->score * 2);
    }
}

void Game::spawnBullet(std::shared_ptr<Entity> entity, const Vec2& target)
{
    
}

void Game::spawnSpecialWeapon(std::shared_ptr<Entity> entity)
{

}

void Game::sMovement()
{
    // determine player velocity according to cInput
    Vec2 playerVelocity;
    if (m_player->cInput->left)
        playerVelocity.x -= m_playerConfig.S;
    if (m_player->cInput->right)
        playerVelocity.x += m_playerConfig.S;
    if (m_player->cInput->up)
        playerVelocity.y -= m_playerConfig.S;
    if (m_player->cInput->down)
        playerVelocity.y += m_playerConfig.S;
    if (playerVelocity.x != 0.0f && playerVelocity.y != 0.0f)
        // moving diagonally, so scale each component by 1/(sqrt 2)
        // so that (v.x)**2 + (v.y)**2 == m_playerConfig.S
        playerVelocity /= sqrt2;
    m_player->cTransform->velocity = playerVelocity;

    // move everything
    for (auto e : m_entities.getEntities())
    {
        e->cTransform->pos += e->cTransform->velocity;
        // player can't leave the bounds
        // enemies must bounce off walls
        auto [flipX, flipY] = detectBorderCollision(e);

    }
}

void Game::sLifespan()
{
    for (auto e : m_entities.getEntities())
    {
        if (!e->cLifespan) continue;

        if (e->cLifespan->remaining > 0)
        {
            // decrease the lifespan
            e->cLifespan->remaining -= 1;
        }

        if (e->cLifespan->remaining == 0)
        {
            // just died
            e->destroy();
        }
        else if (e->isActive() && e->cShape)
        {
            // alive and active
            // scale alpha
            float x = 255.0f / static_cast<float>(e->cLifespan->total);
            int alpha = round(x*static_cast<float>(e->cLifespan->remaining));
            if (alpha > 255) alpha = 255;
            auto color = e->cShape->circle.getFillColor();
            e->cShape->circle.setFillColor(sf::Color(color.r, color.g, color.b, alpha));
            color = e->cShape->circle.getOutlineColor();
            e->cShape->circle.setOutlineColor(sf::Color(color.r, color.g, color.b, alpha));
        }
    }
}

void Game::sCollision()
{
    // between player and enemy
    bool shouldSpawnPlayer = false;
    for (auto p : m_entities.getEntities("player"))
    {
        for (auto e : m_entities.getEntities("enemy"))
        {
            if (areColliding(p, e))
            {
                p->destroy();
                e->destroy();
            }
        }
        for (auto e : m_entities.getEntities("small"))
        {
            if (areColliding(p, e))
            {
                p->destroy();
                e->destroy();
            }
        }
        if (!p->isActive())
        {
            shouldSpawnPlayer = true;
        }
    }
    if (shouldSpawnPlayer)
    {
        spawnPlayer();
    }

    // between enemy and bullet
    for (auto b : m_entities.getEntities("bullet"))
    {
        for (auto e : m_entities.getEntities("enemy"))
        {
            if (areColliding(b, e))
            {
                b->destroy();
                e->destroy();
            }
        }
        for (auto e : m_entities.getEntities("small"))
        {
            if (areColliding(b, e))
            {
                b->destroy();
                e->destroy();
            }
        }
    }
}

void Game::sEnemySpawner()
{

}

void Game::sUserInput()
{

}

void Game::sRender()
{

}

// get number in range [a, b]
int random(const int a, const int b)
{
    return a + (rand() % (1 + b - a));
}

// get number in range [a, b]
float random(const float a, const float b)
{
    return a + static_cast<float>(rand()) / static_cast<float>(RAND_MAX / (b - a));
}

bool areColliding(const std::shared_ptr<Entity> e1, const std::shared_ptr<Entity> e2)
{
    return false;
}

std::pair<bool, bool> detectBorderCollision(const std::shared_ptr<Entity> e)
{
    return std::make_pair(false, false);
}