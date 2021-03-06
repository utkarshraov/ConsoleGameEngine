#pragma once
#include "SpriteEditor.h"
using namespace std;
// Define our static variables from the game engine
std::atomic<bool> ConsoleGameEngine::atomActive(false);
std::condition_variable ConsoleGameEngine::gameFinished;
std::mutex ConsoleGameEngine::muxGame;



int main()
{
	srand(clock());
	SpriteEditor game;
	game.ConstructConsole(160, 100, 8, 8);
	game.StartThread();
	return 0;
}

