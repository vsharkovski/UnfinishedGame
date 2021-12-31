#include <direct.h>
#include <stdio.h>

#include <iostream>
#include <SFML/Graphics.hpp>

int main()
{
	printf("%s\n", _getcwd(NULL, 0));

	sf::Texture t;
	if (!t.loadFromFile("images/mario/tempblock.png"))
	{
		std::cout << "Failed to load." << std::endl;
	}
	else {
		std::cout << "Successfully loaded." << std::endl;
	}
	return 0;
}