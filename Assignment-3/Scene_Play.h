#pragma once

#include "Common.h"
#include "Scene.h"
#include "EntityManager.h"

class Scene_Play :
    public Scene
{
    struct PlayerConfig
    {
        float GX=0, GY=0, CW=0, CH=0, SPEED=0, MAXSPEED=0, JUMP=0, GRAVITY=0;
        std::string WEAPON;
    };

protected:
    std::shared_ptr<Entity> m_player;
    std::string m_levelPath;
    PlayerConfig m_playerConfig;
    bool m_drawTextures = true;
    bool m_drawCollision = false;
    bool m_drawGrid = false;
    const Vec2 m_gridSize = { 64.0f, 64.0f };
    sf::Text m_gridText;

    void init(const std::string& levelPath);
    
    void loadLevel(const std::string& levelPath);
    Vec2 gridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity);
    void drawLine(const Vec2& p1, const Vec2& p2);

    void spawnPlayer();
    void spawnBullet(std::shared_ptr<Entity> entity);
    void onEnd();

    void sAnimation();
    void sMovement();
    void sLifespan();
    void sCollision();
    void sRender();
    void sDoAction(const Action& action);
    //void sDebug();

public:
    Scene_Play(GameEngine* gameEngine, const std::string& levelPath);
    void update();
};

