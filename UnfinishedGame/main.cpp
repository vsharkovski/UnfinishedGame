#include "GameEngine.h"

#include <direct.h>
#include <stdio.h>

int main()
{
	printf("WORKING DIRECTORY: %s\n", _getcwd(NULL, 0));

	GameEngine g("config/assets.txt");
	g.run();
	return 0;
}