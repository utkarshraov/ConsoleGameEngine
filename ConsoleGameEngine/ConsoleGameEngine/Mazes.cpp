#pragma once
#include "Mazes.h"


using namespace std;
// Define our static variables
std::atomic<bool> ConsoleGameEngine::atomActive(false);
std::condition_variable ConsoleGameEngine::gameFinished;
std::mutex ConsoleGameEngine::muxGame;
