#include <SFML/Graphics.hpp>
#include <iostream>
#include "Vec2.h"

int main()
{
    Vec2 p1(10, 20);
    Vec2 p2(30, 10);
    Vec2 p3 = p1 + p2;
    p3.scale(100);
    std::cout << p3.x << " " << p3.y << std::endl;
    std::cout << p3.dist(Vec2(0, 0)) << std::endl;
    return 0;
}