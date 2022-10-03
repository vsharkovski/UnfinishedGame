#pragma once

#include "Common.h"
#include "Scene.h"
#include "Physics.h"
#include "ParticleSystem.h"

class Scene_Level :
    public Scene
{
protected:
    bool m_drawTextures = true;
    bool m_drawCollision = false;
    bool m_drawParticles = true;

    ParticleSystem m_particleSystem;

    Vec2 m_mousePos;
    
    const float m_cameraSpeed = 8.0f;
    Entity m_camera;

    void init(const std::string& levelPath);
    void onEnd();
    void loadLevel(const std::string& path);

    void sDoAction(const Action& action);
    void sRender();
    void sMovement();
    void sAnimation();
    void sCamera();

public:
    Scene_Level(GameEngine* gameEngine, const std::string& levelPath);
    void update();
};

