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
    
    GUI::VerticalTextOptions m_editorOptions;
    std::vector<std::pair<std::string, std::string>> m_levels;

    void init();
    void onEnd();

    void sRender();
    void sDoAction(const Action& action);

    void loadLevelList(const std::string& path);

public:
    Scene_Menu(GameEngine* gameEngine);
    void update();
};
