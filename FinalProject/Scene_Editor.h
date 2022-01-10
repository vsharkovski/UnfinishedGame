#pragma once

#include "Common.h"
#include "Scene.h"

class Scene_Editor :
    public Scene
{
protected:

    bool m_drawTextures = true;
    bool m_drawCollision = false;

    void init();
    void onEnd();

    void sRender();
    void sDoAction(const Action& action);
    void sAnimation();

public:
    Scene_Editor(GameEngine* gameEngine);
    void update();
};

