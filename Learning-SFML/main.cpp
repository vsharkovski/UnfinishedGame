#include <SFML/Graphics.hpp>
#include <iostream>
#include "Vec2.h"

int main()
{
    Vec2 p1(100, 200);
    Vec2 p2(50, -50);
    Vec2 p3 = p1 + p2;
    std::cout << p3.x << " " << p3.y << std::endl;

    return 0;
}