#pragma once

#include "Common.h"
#include "Scene.h"
#include "Physics.h"

class Scene_Editor :
    public Scene
{
protected:
    bool m_drawTextures = true;
    bool m_drawCollision = false;
    bool m_drawGUI = true;

    const float m_cameraSpeed = 8.0f;
    Entity m_camera;

    Vec2 m_mousePos;
    int m_draggingCount = 0;
    
    std::map<std::string, std::string> m_animationToTag;

    void init();
    void onEnd();

    void sDoAction(const Action& action);
    void sRender();
    void sRenderDrawEntity(Entity e);
    void sRenderDrawCollision(Entity e);
    
    void sMovement();
    void sDragging();
    void sCollision();
    void sClicking();
    void sAnimation();
    void sCamera();

public:
    Scene_Editor(GameEngine* gameEngine);
    void update();
};

