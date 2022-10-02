#include "Scene_Menu.h"
#include "Scene_Level.h"
#include "Scene_Editor.h"

Scene_Menu::Scene_Menu(GameEngine* gameEngine)
	: Scene(gameEngine)
{
	init();
}

void Scene_Menu::init()
{
	registerAction(sf::Keyboard::Escape, "BACK");
	registerAction(sf::Keyboard::Backspace, "BACK");
	registerAction(sf::Keyboard::Space, "CONFIRM");
	registerAction(sf::Keyboard::Enter, "CONFIRM");
	registerAction(sf::Keyboard::D, "CONFIRM");
	registerAction(sf::Keyboard::W, "UP");
	registerAction(sf::Keyboard::Up, "UP");
	registerAction(sf::Keyboard::S, "DOWN");
	registerAction(sf::Keyboard::Down, "DOWN");

	loadLevelList("levels/levelList.txt");

	auto& consolasFont = m_game->assets().getFont("Consolas");

	m_titleText.setFont(consolasFont);
	m_titleText.setCharacterSize(80);
	m_titleText.setFillColor(sf::Color::Green);
	m_titleText.setString("COOL GAME");
	m_titleText.setPosition(Vec2(200.0f, 80.0f));

	sf::Text baseText;
	baseText.setFont(consolasFont);
	baseText.setCharacterSize(40);
	baseText.setFillColor(sf::Color::White);

	m_menuOptions.setItems({
		{ "play", std::make_shared<GUI::Text>(baseText, "PLAY") },
		{ "editor", std::make_shared<GUI::Text>(baseText, "EDITOR") },
		{ "options", std::make_shared<GUI::Text>(baseText, "OPTIONS") },
		{ "exit", std::make_shared<GUI::Text>(baseText, "EXIT") }
	});
	m_menuOptions.setPosition(m_titleText.positionUnder() + Vec2(0, 50.0f));

	std::vector<std::pair<std::string, std::shared_ptr<GUI::Text>>> editorOptionsItems;
	for (auto& item : m_levels)
	{
		std::string text = item.first + " : " + item.second;
		editorOptionsItems.push_back({ item.first, std::make_shared<GUI::Text>(baseText, text) });
	}
	m_editorOptions.setItems(editorOptionsItems);
	m_editorOptions.setPosition(m_titleText.positionUnder() + Vec2(0, 50.0f));

	m_currentScreen = "menu";
}

void Scene_Menu::update()
{
	m_currentFrame++;
}

void Scene_Menu::onEnd()
{
	m_game->quit();
}

void Scene_Menu::loadLevelList(const std::string& listPath)
{
	std::ifstream file(listPath);
	if (!file.is_open())
	{
		std::cerr << "Could not load level list from file: " << listPath << std::endl;
		return;
	}

	m_levels.clear();

	std::string name, path;
	while (getline(file, name))
	{
		getline(file, path);
		std::cout << "Name='" << name << "', Path='" << path << "'" << std::endl;
		m_levels.push_back({ name, path });
	}
}

void Scene_Menu::sDoAction(const Action& action)
{
	if (action.type() == "START")
	{
		if (action.name() == "BACK")
		{
			if (m_currentScreen == "menu") { onEnd(); }
			else { m_currentScreen = "menu"; }
		}
		else if (action.name() == "CONFIRM")
		{
			if (m_currentScreen == "menu")
			{
				std::string selected = m_menuOptions.selected().first;
				if (selected == "play")
				{
					m_game->changeScene("LEVEL", std::make_shared<Scene_Level>(m_game));
				}
				else if (selected == "editor")
				{
					m_currentScreen = "editor";
				}
				else if (selected == "exit")
				{
					onEnd();
				}
				else if (selected == "options") {}
			}
			else if (m_currentScreen == "editor")
			{
				std::string name = m_editorOptions.selected().first;
				auto level = std::find_if(m_levels.begin(), m_levels.end(), [&](auto& it) { return it.first == name; });
				if (level != m_levels.end())
				{
					m_game->changeScene("EDITOR", std::make_shared<Scene_Editor>(m_game, level->second));
				}
			}
			else if (m_currentScreen == "options")
			{

			}
		}
		else if (action.name() == "DOWN")
		{
			if (m_currentScreen == "menu")
			{
				m_menuOptions.selectNext();
			}
			else if (m_currentScreen == "editor")
			{
				m_editorOptions.selectNext();
			}
		}
		else if (action.name() == "UP")
		{
			if (m_currentScreen == "menu")
			{
				m_menuOptions.selectPrevious();
			}
			else if (m_currentScreen == "editor")
			{
				m_editorOptions.selectPrevious();
			}
		}
	}
	else if (action.type() == "END")
	{

	}
	else if (action.name() == "MOUSE_MOVE")
	{
		
	}
}

void Scene_Menu::sRender()
{
	// reset view to normal
	sf::View view = m_game->window().getView();
	view.setCenter(static_cast<float>(width()) / 2.0f, static_cast<float>(height()) / 2.0f);
	m_game->window().setView(view);

	// draw text
	m_game->window().clear(sf::Color(0, 0, 0));
	
	m_titleText.draw(m_game->window());
	
	if (m_currentScreen == "menu")
	{
		m_menuOptions.draw(m_game->window());
	}
	else if (m_currentScreen == "editor")
	{
		m_editorOptions.draw(m_game->window());
	}
	else if (m_currentScreen == "options")
	{

	}
}