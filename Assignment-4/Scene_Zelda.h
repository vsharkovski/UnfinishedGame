#pragma once

#include "Common.h"
#include "Scene.h"
#include "EntityManager.h"

class Scene_Zelda :
    public Scene
{

    struct PlayerConfig
    {
        float X, Y, CW, CH, SPEED, HEALTH;
    };

protected:
    std::shared_ptr<Entity> m_player;
    std::string m_levelPath;
    PlayerConfig m_playerConfig;
    bool m_drawTextures = true;
    bool m_drawCollision = false;
    bool m_drawGrid = false;
    const Vec2 m_roomSize = { 20.0f, 12.0f };
    const Vec2 m_tileSize = { 64.0f, 64.0f };

    void init(const std::string& levelPath);
    void onEnd();

    void loadLevel(const std::string& levelPath);
    Vec2 gridToMidPixel(const Vec2& room, const Vec2& tile, std::shared_ptr<Entity> entity);

    void sRender();
    void sDoAction(const Action& action);
    void sAnimation();
    void sMovement();
    void sCollision();
 
public:
    Scene_Zelda(GameEngine* gameEngine, const std::string& levelPath);
    void update();
};

