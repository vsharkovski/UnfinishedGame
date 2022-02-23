#pragma once

#include "Common.h"
#include "Scene.h"

class Scene_Menu :
    public Scene
{
protected:
    sf::Text m_menuText;
    std::string m_title;

    std::string m_currentScreen;
    size_t m_selectedIndex;
    std::vector<std::string> m_menuItems;

    void init();
    void onEnd();

    void sRender();
    void sDoAction(const Action& action);

public:
    Scene_Menu(GameEngine* gameEngine);
    void update();

};

