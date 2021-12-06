#include <SFML/Graphics.hpp>
#include <iostream>
#include "Vec2.h"

int main()
{
    Vec2 p1(100, 200);
    Vec2 p2(50, -50);
    Vec2 p3 = p1 + p2;
    p3.scale(10);
    std::cout << p3.x << " " << p3.x << std::endl;
    return 0;
}