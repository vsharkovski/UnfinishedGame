#pragma once

#include "Common.h"
#include "Scene.h"
#include "VerticalTextOptions.h"

class Scene_Menu :
    public Scene
{
protected:
    std::string m_currentScreen;

    GUI::Text m_titleText;
    GUI::VerticalTextOptions m_menuOptions;

    void init();
    void onEnd();

    void sRender();
    void sDoAction(const Action& action);

public:
    Scene_Menu(GameEngine* gameEngine);
    void update();
};
