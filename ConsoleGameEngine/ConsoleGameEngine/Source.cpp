#pragma once
#include "Mazes.h"


int main()
{
	Maze game;
	game.ConstructConsole(160, 100, 8, 8);
	game.StartThread();
	return 0;
}

