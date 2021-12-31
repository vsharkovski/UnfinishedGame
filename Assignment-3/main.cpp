#include "GameEngine.h"

#include <direct.h>
#include <stdio.h>

int main()
{
	printf("%s\n", _getcwd(NULL, 0));

	GameEngine g("config/assetsDebug.txt");
	g.run();
	return 0;
}