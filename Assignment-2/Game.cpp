#include "Game.h"

/*
    TODO: IMPLEMENT CHILL MECHANIC
    (if you want)
*/

const float pi = 3.14159265358979f;
const float sqrt2 = sqrt(2.0f);

int random(const int a, const int b);
float random(const float a, const float b);
bool areColliding(const std::shared_ptr<Entity> e1, const std::shared_ptr<Entity> e2);

Game::Game(const std::string& config)
{
    init(config);
}

void Game::init(const std::string& path)
{
    srand(static_cast<unsigned int>(time(0)));

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
            m_text = sf::Text("Score: 0", m_font, fontSize);
            m_text.setFillColor(sf::Color(r, g, b));
            m_text.setPosition(10.0f, 10.0f);
        }
        else if (command == "Player")
        {
            file    >> m_playerConfig.SR >> m_playerConfig.CR
                    >> m_playerConfig.S
                    >> m_playerConfig.FR >> m_playerConfig.FG >> m_playerConfig.FB
                    >> m_playerConfig.OR >> m_playerConfig.OG >> m_playerConfig.OB
                    >> m_playerConfig.OT
                    >> m_playerConfig.V;
        }
        else if (command == "Enemy")
        {
            file    >> m_enemyConfig.SR >> m_enemyConfig.CR
                    >> m_enemyConfig.SMIN >> m_enemyConfig.SMAX
                    >> m_enemyConfig.OR >> m_enemyConfig.OG >> m_enemyConfig.OB
                    >> m_enemyConfig.OT
                    >> m_enemyConfig.VMIN >> m_enemyConfig.VMAX
                    >> m_enemyConfig.L
                    >> m_enemyConfig.SI;
        }
        else if (command == "Bullet")
        {
            file    >> m_bulletConfig.SR >> m_bulletConfig.CR
                    >> m_bulletConfig.S
                    >> m_bulletConfig.FR >> m_bulletConfig.FG >> m_bulletConfig.FB
                    >> m_bulletConfig.OR >> m_bulletConfig.OG >> m_bulletConfig.OB
                    >> m_bulletConfig.OT
                    >> m_bulletConfig.V
                    >> m_bulletConfig.L;
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
        if (m_paused)
        {

        }
        else
        {
            m_entities.update();
            sEnemySpawner();
            sMovement();
            sCollision();
            sLifespan();
            sBurstShooting();
        }
        sRender();
        sUserInput();
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
        Vec2(0.0f, 0.0f), 0.0f);
    entity->cShape = std::make_shared<CShape>(
        m_playerConfig.SR,
        m_playerConfig.V,
        sf::Color(m_playerConfig.FR, m_playerConfig.FG, m_playerConfig.FB),
        sf::Color(m_playerConfig.OR, m_playerConfig.OG, m_playerConfig.OB),
        m_playerConfig.OT);
    entity->cCollision = std::make_shared<CCollision>(m_playerConfig.CR);
    entity->cInput = std::make_shared<CInput>();
    entity->cBurst = std::make_shared<CBurst>(3, pi / 8.0f, 20, 0, 0);
    m_player = entity;
}

void Game::spawnEnemy()
{
    float margin = static_cast<float>(m_enemyConfig.SR);
    float spawnX = random(margin, static_cast<float>(m_window.getSize().x) - margin);
    float spawnY = random(margin, static_cast<float>(m_window.getSize().y) - margin);

    float speed = random(m_enemyConfig.SMIN, m_enemyConfig.SMAX);
    float angle = random(0.0f, 2.0f * pi);
    Vec2 velocity(speed * cos(angle), speed * sin(angle));

    int vertices = random(m_enemyConfig.VMIN, m_enemyConfig.VMAX);

    auto entity = m_entities.addEntity("enemy");
    entity->cTransform = std::make_shared<CTransform>(
        Vec2(spawnX, spawnY), velocity, 0.0f);
    entity->cShape = std::make_shared<CShape>(
        m_enemyConfig.SR,
        vertices,
        sf::Color(random(0, 255), random(0, 255), random(0, 255)),
        sf::Color(m_enemyConfig.OR, m_enemyConfig.OG, m_enemyConfig.OB),
        m_enemyConfig.OT);
    entity->cCollision = std::make_shared<CCollision>(m_enemyConfig.CR);
    entity->cScore = std::make_shared<CScore>(vertices * 100);
    entity->cChill = std::make_shared<CChill>(false);

    m_lastEnemySpawnTime = m_currentFrame;
}

void Game::spawnSmallEnemies(const std::shared_ptr<Entity> e)
{
    int vertices = e->cShape->circle.getPointCount();
    float baseAngle = pi * 2.0f / static_cast<float>(vertices);
    float baseSpeed = e->cTransform->velocity.dist();
    for (int i = 0; i < vertices; i++)
    {
        float angle = baseAngle * static_cast<float>(i);
        Vec2 velocity(baseSpeed * cos(angle), baseSpeed * sin(angle));

        auto small = m_entities.addEntity("small");
        small->cTransform = std::make_shared<CTransform>(
            e->cTransform->pos, velocity, 0.0f);
        small->cShape = std::make_shared<CShape>(
            e->cShape->circle.getRadius() / 2.0f,
            vertices,
            e->cShape->circle.getFillColor(),
            e->cShape->circle.getOutlineColor(),
            e->cShape->circle.getOutlineThickness());
        small->cCollision = std::make_shared<CCollision>(e->cCollision->radius / 2.0f);
        small->cLifespan = std::make_shared<CLifespan>(m_enemyConfig.L);
        small->cScore = std::make_shared<CScore>(e->cScore->score * 2);
        small->cChill = std::make_shared<CChill>(e->cChill->chilled);
    }
}

void Game::spawnBullet(std::shared_ptr<Entity> entity, const Vec2& target, const bool chill)
{
    Vec2 diff = target - entity->cTransform->pos;
    diff /= diff.dist(); // normalize so it has length 1
    Vec2 velocity(diff.x * m_bulletConfig.S, diff.y * m_bulletConfig.S);

    auto bullet = m_entities.addEntity("bullet");
    bullet->cTransform = std::make_shared<CTransform>(
        entity->cTransform->pos, velocity, 0.0f);
    bullet->cShape = std::make_shared<CShape>(
        m_bulletConfig.SR,
        m_bulletConfig.V,
        sf::Color(m_bulletConfig.FR, m_bulletConfig.FG, m_bulletConfig.FB),
        sf::Color(m_bulletConfig.OR, m_bulletConfig.OG, m_bulletConfig.OB),
        m_bulletConfig.OT);
    bullet->cCollision = std::make_shared<CCollision>(m_bulletConfig.CR);
    bullet->cLifespan = std::make_shared<CLifespan>(m_bulletConfig.L);
    bullet->cChill = std::make_shared<CChill>(chill);
}

void Game::startBurst(std::shared_ptr<Entity> entity)
{
    if (!entity->cBurst)
        return;
    std::cout << "Starting burst" << std::endl;
    entity->cBurst->burstsRemaining = 3;
    entity->cBurst->cooldownRemaining = 0;
    m_lastSpecialTime = m_currentFrame;
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
        float r = e->cCollision->radius;
        bool colX = e->cTransform->pos.x - r < 0 || e->cTransform->pos.x + r > m_window.getSize().x;
        bool colY = e->cTransform->pos.y - r < 0 || e->cTransform->pos.y + r > m_window.getSize().y;
        if (colX || colY)
        {
            if (e->tag() == "player")
            {
                // put back into bounds
                if (colX)
                    e->cTransform->pos.x -= e->cTransform->velocity.x;
                if (colY)
                    e->cTransform->pos.y -= e->cTransform->velocity.y;
            }
            else if (e->tag() == "enemy" || e->tag() == "small")
            {
                // put back into bounds, flip velocity
                e->cTransform->pos -= e->cTransform->velocity;
                if (colX)
                    e->cTransform->velocity.x *= (-1.0f);
                if (colY)
                    e->cTransform->velocity.y *= (-1.0f);
                e->cTransform->pos += e->cTransform->velocity;
            }
        }
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
            int alpha = static_cast<int>(round(x*static_cast<float>(e->cLifespan->remaining)));
            auto color = e->cShape->circle.getFillColor();
            e->cShape->circle.setFillColor(sf::Color(color.r, color.g, color.b, alpha));
            color = e->cShape->circle.getOutlineColor();
            e->cShape->circle.setOutlineColor(sf::Color(color.r, color.g, color.b, alpha));
        }
    }
}

void Game::sBurstShooting()
{
    for (auto p : m_entities.getEntities("player"))
    {
        if (!p->cBurst || p->cBurst->burstsRemaining == 0)
            continue;
        if (p->cBurst->cooldownRemaining > 0)
            p->cBurst->cooldownRemaining -= 1;
        if (p->cBurst->cooldownRemaining == 0)
        {
            p->cBurst->cooldownRemaining = p->cBurst->cooldownTotal;
            p->cBurst->burstsRemaining -= 1;

            // shoot burst
            sf::Vector2i mousePos = sf::Mouse::getPosition(m_window);
            float mouseAngle = atan2(
                static_cast<float>(mousePos.y) - p->cTransform->pos.y,
                static_cast<float>(mousePos.x) - p->cTransform->pos.x);

            float delta = p->cBurst->sprayAngle / static_cast<float>(p->cBurst->sprayColumns);
            float angle;
            if (p->cBurst->sprayColumns % 2 == 0)
                angle = mouseAngle - delta * (static_cast<float>(p->cBurst->sprayColumns / 2) - 0.5f);
            else
                angle = mouseAngle - delta * static_cast<float>((p->cBurst->sprayColumns - 1) / 2);

            for (int i = 0; i < p->cBurst->sprayColumns; ++i)
            {
                Vec2 target = p->cTransform->pos + Vec2(cos(angle), sin(angle));
                spawnBullet(p, target, true);
                angle += delta;
            }
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
                spawnSmallEnemies(e);
            }
        }
        for (auto e : m_entities.getEntities("small"))
        {
            if (areColliding(p, e))
            {
                p->destroy();
                e->destroy();
                //spawnSmallEnemies(e);
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
                spawnSmallEnemies(e);
                m_score += e->cScore->score;
            }
        }
        for (auto e : m_entities.getEntities("small"))
        {
            if (areColliding(b, e))
            {
                b->destroy();
                e->destroy();
                //spawnSmallEnemies(e);
                m_score += e->cScore->score;
            }
        }
    }
}

void Game::sEnemySpawner()
{
    if (m_lastEnemySpawnTime + m_enemyConfig.SI < m_currentFrame)
    {
        spawnEnemy();
    }
}

void Game::sRender()
{
    m_window.clear();
    
    for (auto e : m_entities.getEntities())
    {
        // set position based on transform.pos
        e->cShape->circle.setPosition(e->cTransform->pos.x, e->cTransform->pos.y);
        // set rotation based on transform.angle
        e->cTransform->angle += 1.0f;
        e->cShape->circle.setRotation(e->cTransform->angle);
        // draw the entity
        m_window.draw(e->cShape->circle);
    }
    
    m_text.setString("Score: " + std::to_string(m_score));
    m_window.draw(m_text);

    m_window.display();
}

void Game::sUserInput()
{
    sf::Event event;
    while (m_window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            // window was closed
            m_running = false;
        }
        else if (event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased)
        {
            bool isDown = event.type == sf::Event::KeyPressed;

            switch (event.key.code)
            {
            case sf::Keyboard::W:
                m_player->cInput->up = isDown;
                break;
            case sf::Keyboard::S:
                m_player->cInput->down = isDown;
                break;
            case sf::Keyboard::A:
                m_player->cInput->left = isDown;
                break;
            case sf::Keyboard::D:
                m_player->cInput->right = isDown;
                break;
            case sf::Keyboard::P:
                if (isDown)
                    m_paused = !m_paused;
                break;
            case sf::Keyboard::Escape:
                m_running = false;
                break;
            }
        }
        else if (event.type == sf::Event::MouseButtonPressed)
        {
            if (!m_paused)
            {
                Vec2 mousePos(
                    static_cast<float>(event.mouseButton.x),
                    static_cast<float>(event.mouseButton.y));
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    spawnBullet(m_player, mousePos, false);
                }
                else if (event.mouseButton.button == sf::Mouse::Right)
                {
                    if (m_lastSpecialTime + m_specialCooldown < m_currentFrame)
                    {
                        startBurst(m_player);
                    }
                }
            }
        }
    }
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

bool areColliding(std::shared_ptr<Entity> e1, std::shared_ptr<Entity> e2)
{
    return e1->cTransform->pos.dist(e2->cTransform->pos) < e1->cCollision->radius + e2->cCollision->radius;
}
