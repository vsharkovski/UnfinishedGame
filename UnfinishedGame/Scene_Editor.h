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
    bool m_draggingBlockMove = false;
    bool m_draggingBlockVision = false;

    std::string m_levelPath;

    void init(const std::string& levelPath);
    void onEnd();
    void loadLevel(const std::string& path);
    void saveLevel(const std::string& path);

    void sDoAction(const Action& action);
    void sRender();
    void sRenderDrawGUI();
    void sRenderDrawEntity(Entity e);
    void sRenderDrawCollision(Entity e);
    
    void sMovement();
    void sDragging();
    void sClicking();
    void sAnimation();
    void sCamera();

    void initEditorEntities();
    Entity createEntityFromGuiTemplate(Entity templ);

public:
    Scene_Editor(GameEngine* gameEngine, const std::string& levelPath);
    void update();
};

