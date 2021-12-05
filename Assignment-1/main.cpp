#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <memory>


struct MovingShape
{
    std::shared_ptr<sf::Shape> shape;
    std::shared_ptr<sf::Text> text;
    sf::Vector2f velocity;
    bool centeredOnce = false;

    MovingShape(std::shared_ptr<sf::Shape> inshape, std::shared_ptr<sf::Text> intext, float sx, float sy)
        : shape(inshape), text(intext), velocity(sx, sy)
    {
        centerText();
    }

    void move(float wWidth, float wHeight)
    {
        sf::Vector2f oldPosition = shape->getPosition();
        shape->setPosition(oldPosition + velocity);

        sf::FloatRect bounds = shape->getGlobalBounds();
        bool flipX = (bounds.left < 0.0f || bounds.left + bounds.width > wWidth);
        bool flipY = (bounds.top < 0.0f || bounds.top + bounds.height > wHeight);

        if (flipX || flipY) {
            velocity = sf::Vector2f(
                flipX ? velocity.x * (-1.0f) : velocity.x,
                flipY ? velocity.y * (-1.0f) : velocity.y
            );

            shape->setPosition(oldPosition + velocity);
        }

        centerText();
    }

    void centerText()
    {
        if (centeredOnce) {
            return;
        }
        centeredOnce = true;

        sf::FloatRect shapeBounds = shape->getGlobalBounds();
        sf::Vector2f shapeCenter(
            shapeBounds.left + shapeBounds.width * 0.5f, 
            shapeBounds.top + shapeBounds.height * 0.5f
        );
        std::cout << "text position=(" << text->getPosition().x << "," << text->getPosition().y << ")" << std::endl;
        sf::FloatRect textBounds = text->getLocalBounds();
        text->setPosition(sf::Vector2f(
            shapeCenter.x - textBounds.width * 0.5f,
            shapeCenter.y - textBounds.height * 0.5f
        ));
    }
};


void readConfig(int& wWidth, int& wHeight, std::vector<MovingShape>& shapes)
{
    std::ifstream configFile("config.txt");
    if (!configFile.is_open())
    {
        std::cerr << "Could not open config file." << std::endl;
        return;
    }

    sf::Font myFont;
    sf::Color textColor;
    int textSize;

    std::string command;
    while (configFile >> command) {
        if (command == "STOP")
        {
            return;
        }
        else if (command == "Window")
        {
            configFile >> wWidth >> wHeight;
        }
        else if (command == "Font")
        {
            std::string path;
            int r, g, b;
            configFile >> path >> textSize >> r >> g >> b;
            if (!myFont.loadFromFile(path))
            {
                std::cerr << "Could not load font!" << std::endl;
                return;
            }
            textColor = sf::Color(r, g, b);
            std::cout << "textColor=(" << r << "," << g << "," << b << ")" << std::endl;
        }
        else if (command == "Rectangle")
        {
            std::string name;
            int r, g, b;
            float x, y, sx, sy, w, h;
            configFile >> name >> x >> y >> sx >> sy >> r >> g >> b >> w >> h;

            auto rect = std::make_shared<sf::RectangleShape>(sf::Vector2f(w, h));
            rect->setFillColor(sf::Color(r, g, b));
            rect->setPosition(sf::Vector2f(x, y));

            auto text = std::make_shared<sf::Text>(name, myFont, textSize);
            text->setFillColor(textColor);
            text->setPosition(sf::Vector2f(x, y));

            shapes.push_back(MovingShape(rect, text, sx, sy));
        }
        else if (command == "Circle")
        {
            std::string name;
            int r, g, b;
            float x, y, sx, sy, radius;
            configFile >> name >> x >> y >> sx >> sy >> r >> g >> b >> radius;

            auto circ = std::make_shared<sf::CircleShape>(radius);
            circ->setFillColor(sf::Color(r, g, b));
            circ->setPosition(sf::Vector2f(x, y));

            auto text = std::make_shared<sf::Text>(name, myFont, textSize);
            text->setFillColor(textColor);
            text->setPosition(sf::Vector2f(x, y));

            shapes.push_back(MovingShape(circ, text, sx, sy));
        }
    }
}


int main()
{
    int wWidth = 640;
    int wHeight = 480;
    std::vector<MovingShape> shapes;

    readConfig(wWidth, wHeight, shapes);
    
    sf::RenderWindow window(sf::VideoMode(wWidth, wHeight), "Assignment 1");

    while (window.isOpen())
    {
        for (auto& shape : shapes)
        {
            shape.move((float)wWidth, (float)wHeight);
        }

        window.clear();
        for (auto& shape : shapes)
        {
            window.draw(*shape.shape);
            window.draw(*shape.text);
        }
        window.display();
    }

    return 0;
}