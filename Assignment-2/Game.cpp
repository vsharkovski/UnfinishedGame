#include "Game.h"
#include <fstream>
#include <iostream>

Game::Game()
{
	// read config file
    std::ifstream configFile("config.txt");
    if (!configFile.is_open())
    {
        std::cerr << "Could not open config file." << std::endl;
        exit(-1);
    }

    int wWidth = 600;
    int wHeight = 480;
    int frameLimit, isFullScreen;
   
    std::string command;
    while (configFile >> command)
    {
        if (command == "Window")
        {
            configFile >> wWidth >> wHeight >> frameLimit >> isFullScreen;
        }
        else if (command == "Font")
        {
            std::string path;
            int r, g, b;
            configFile >> path >> m_fontSize >> r >> g >> b;
            if (!m_font.loadFromFile(path))
            {
                std::cerr << "Could not load font!" << std::endl;
                exit(-1);
            }
            m_fontColor = sf::Color(r, g, b);
        }
        else
        {
            int shapeRadius, collisionRadius;
            float speed, minSpeed, maxSpeed;
            int fillR, fillG, fillB;
            int outR, outG, outB;
            int outThickness;
            int vertices, minVertices, maxVertices;
            int lifespan;
            int spawnInterval;
            if (command == "Player")
            {
                configFile >> shapeRadius >> collisionRadius >> speed >> fillR >> fillG >> fillB >> outR >> outG >> outB >> outThickness >> vertices;

            }
            else if (command == "Enemy")
            {
                configFile >> shapeRadius >> collisionRadius >> minSpeed >> maxSpeed >> outR >> outG >> outB >> outThickness >> minVertices >> maxVertices >> lifespan >> spawnInterval;

            }
            else if (command == "Bullet")
            {
                configFile >> shapeRadius >> collisionRadius >> speed >> fillR >> fillG >> fillB >> outR >> outG >> outB >> outThickness >> vertices >> lifespan;
                
            }
        }
    }

    m_window = std::make_shared<sf::RenderWindow>(
        sf::VideoMode(wWidth, wHeight), "Assignment 2");

    m_player = m_entities.addEntity("Player");

}