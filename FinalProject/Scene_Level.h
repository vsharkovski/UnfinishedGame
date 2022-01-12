#pragma once

#include "Common.h"
#include "Scene.h"
#include "Physics.h"

class Scene_Level :
    public Scene
{
protected:
    bool m_drawTextures = true;
    bool m_drawCollision = false;
    int m_drawVisibility = 1;

    Vec2 m_mousePos;
    sf::RenderTexture m_visibilityMask;
    std::vector<Physics::line_segment<Vec2>> m_segments;

    void init();
    void onEnd();

    void sRender();
    void sDoAction(const Action& action);
    void sAnimation();

    void drawLine(const Vec2& p1, const Vec2& p2);
    void computeAllSegments(float offset = 0.0f);

public:
    Scene_Level(GameEngine* gameEngine);
    void update();
};

