#pragma once

#include "Common.h"
#include "Scene.h"
#include "EntityManager.h"

class Scene_Zelda :
    public Scene
{

    struct PlayerConfig
    {
        float X, Y, CW, CH, SPEED;
        int HEALTH;
    };

protected:
    std::string m_levelPath;
    bool m_drawTextures = true;
    bool m_drawCollision = false;
    bool m_drawGrid = false;
    bool m_followCamera = false;
    sf::Text m_gridText;
    const Vec2 m_roomSize = { 20.0f, 12.0f };
    const Vec2 m_tileSize = { 64.0f, 64.0f };
    PlayerConfig m_playerConfig;
    std::shared_ptr<Entity> m_player;

    void init(const std::string& levelPath);
    void loadLevel(const std::string& levelPath);
    void onEnd();

    void sRender();
    void sDoAction(const Action& action);
    void sAnimation();
    void sMovement();
    void sCollision();
    void sAI();
    void sStatus();
    void sAttack();
    void sCamera();
 
    Vec2 getPosition(const Vec2& room, const Vec2& tile, std::shared_ptr<Entity> entity);
    void spawnPlayer();
    void spawnSword(std::shared_ptr<Entity> entity);

public:
    Scene_Zelda(GameEngine* gameEngine, const std::string& levelPath);
    void update();
};

