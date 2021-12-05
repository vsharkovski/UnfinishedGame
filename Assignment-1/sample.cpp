//#include <SFML/Graphics.hpp>
//#include <iostream>
//
//int main()
//{
//    const int wWidth = 640;
//    const int wHeight = 480;
//    sf::RenderWindow window(sf::VideoMode(wWidth, wHeight), "SFML works!");
//
//    sf::CircleShape circle(50);
//    circle.setFillColor(sf::Color(255, 0, 255));
//    circle.setPosition(300.0f, 300.0f);
//    float circleMoveSpeed = -0.01f;
//
//    sf::Font myFont;
//    if (!myFont.loadFromFile("fonts/arial.ttf"))
//    {
//        std::cerr << "Could not load font!" << std::endl;
//        return -1;
//    }
//    
//    sf::Text text("Sample Text", myFont, 24);
//    text.setPosition(0, wHeight - (float)text.getCharacterSize());
//
//    while (window.isOpen())
//    {
//        sf::Event event;
//        while (window.pollEvent(event))
//        {
//            if (event.type == sf::Event::Closed)
//            {
//                window.close();
//            }
//            if (event.type == sf::Event::KeyPressed)
//            {
//                std::cout << "Key pressed with code " << event.key.code << std::endl;
//                if (event.key.code == sf::Keyboard::X)
//                {
//                    circleMoveSpeed *= -1.0f;
//                }
//            }
//        }
//
//        circle.setPosition(circle.getPosition() + sf::Vector2f(0, circleMoveSpeed));
//
//        window.clear();
//        window.draw(circle);
//        window.draw(text);
//        window.display();
//    }
//
//    return 0;
//}