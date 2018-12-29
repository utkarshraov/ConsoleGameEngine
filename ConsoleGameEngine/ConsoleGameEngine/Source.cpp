#pragma once
#include "CirclePhysics.h"
using namespace std;
// Define our static variables from the game engine
std::atomic<bool> ConsoleGameEngine::atomActive(false);
std::condition_variable ConsoleGameEngine::gameFinished;
std::mutex ConsoleGameEngine::muxGame;



int main()
{
	srand(clock());
	CirclePhysics game;
	game.ConstructConsole(160, 120, 8, 8);
	game.StartThread();
	return 0;
}

