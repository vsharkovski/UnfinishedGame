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
    GUI::Text m_creditsText;

    GUI::VerticalTextOptions m_menuOptions;

    std::vector<std::pair<std::string, std::string>> m_levels;
    GUI::VerticalTextOptions m_playOptions;
    GUI::VerticalTextOptions m_editorOptions;
    
    void init();
    void onEnd();

    void sRender();
    void sDoAction(const Action& action);

    void loadLevelList(const std::string& path);

public:
    Scene_Menu(GameEngine* gameEngine);
    void update();
};
