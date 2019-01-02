#pragma once
#include "PerlinNoise.h"
using namespace std;
// Define our static variables from the game engine
std::atomic<bool> ConsoleGameEngine::atomActive(false);
std::condition_variable ConsoleGameEngine::gameFinished;
std::mutex ConsoleGameEngine::muxGame;



int main()
{
	srand(clock());
	PerlinNoise game;
	game.ConstructConsole(256, 256, 3, 3);
	game.StartThread();
	return 0;
}

