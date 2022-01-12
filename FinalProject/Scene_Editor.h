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

    Vec2 m_mousePos;
    bool m_draggingSomething = false;

    const float m_cameraSpeed = 8.0f;
    Entity m_camera;

    void init();
    void onEnd();

    void sRender();
    void sDoAction(const Action& action);
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

