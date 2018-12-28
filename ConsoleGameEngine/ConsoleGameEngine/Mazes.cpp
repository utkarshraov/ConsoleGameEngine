
#include <iostream>
#include "ConsoleGameEngine.h"
#include <stack>

using namespace std;
// A simple maze using Recursive Backtracking

class Maze : public ConsoleGameEngine
{
private:
	int mazeWidth;
	int mazeHeight;
	int *maze;

	enum
	{
		UP = 0x01, RIGHT = 0x02, DOWN = 0x04, LEFT = 0x08, VISITED = 0x10, // enum to represent up, right, down, left and visited as bits 
	};
	int visitedCount;

	stack<pair<int, int>> stack;
public:
	Maze()
	{
		appName = L'Mazes';
	}
protected:
	virtual bool Start()
	{
		//Initialise values
		mazeWidth = 50;
		mazeHeight = 30;

		maze = new int[mazeWidth * mazeHeight];
		memset(maze, 0x00, mazeWidth * mazeHeight);

		stack.push(make_pair(0, 0));
		maze[0] = VISITED;
		visitedCount = 1;
		
		return true;
	}

	virtual bool Update(float elapsedTime)
	{
		return true;
	}
};

int main()
{
	Maze game;
	game.ConstructConsole(240, 160, 8, 8);
	game.StartThread();

	return 0;
}
