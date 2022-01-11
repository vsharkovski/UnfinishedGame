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

    sf::Clock m_clock;
    //sf::Shader m_shader;
    sf::RenderTexture m_visibilityMask;

    std::vector<Physics::line_segment<Vec2>> m_segments;
    Vec2 m_mousePos;

    void init();
    void onEnd();

    void sRender();
    void sDoAction(const Action& action);
    void sAnimation();
    void sDragging();

    void drawLine(const Vec2& p1, const Vec2& p2);
    void computeAllSegments();

public:
    Scene_Editor(GameEngine* gameEngine);
    void update();
};

