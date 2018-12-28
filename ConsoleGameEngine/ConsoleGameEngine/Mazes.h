#pragma once
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
		UP = 0x01, RIGHT = 0x02, DOWN = 0x04, LEFT = 0x08, VISITED = 0x10, // enum to represent up, right, down, left and visited as bits for easy logical checks
	};
	int visitedCount;

	stack<pair<int, int>> blocks;

	int pathWidth;
public:
	Maze()
	{
		appName = L'Mazes';
	}
protected:
	virtual bool Start()
	{
		initRBAlg();
		return true;
	}

	virtual bool Update(float elapsedTime)
	{
		this_thread::sleep_for(10ms);
		executeRBAlg();
		drawRBMaze();
		
		return true;
	}

	private:
		void initRBAlg()
		{
			//Initialise values
			mazeWidth = 40;
			mazeHeight = 25;

			maze = new int[mazeWidth * mazeHeight];
			memset(maze, 0x00, mazeWidth * mazeHeight * sizeof(int));


			int x = rand() % mazeWidth;
			int y = rand() % mazeHeight;
			blocks.push(make_pair(x, y));
			maze[y*mazeWidth + x] = VISITED;
			visitedCount = 1;
			pathWidth = 3;
		}

		void executeRBAlg()
		{
			auto offset = [&](int x, int y)
			{
				return (blocks.top().second + y) * mazeWidth + (blocks.top().first + x);
				//little lambda function to get the annoying offset for me for the bounds checks
			};

			if (visitedCount < mazeWidth * mazeHeight)
			{
				//make list of unvisited neighbours
				vector<int> neighbours;

				//check up
				if (blocks.top().second > 0 && (maze[offset(0, -1)] & VISITED) == 0)
					neighbours.push_back(0);
				//check right
				if (blocks.top().first < mazeWidth - 1 && (maze[offset(1, 0)] & VISITED) == 0)
					neighbours.push_back(1);
				//check down
				if (blocks.top().second < mazeHeight - 1 && (maze[offset(0, 1)] & VISITED) == 0)
					neighbours.push_back(2);
				//check left
				if (blocks.top().first > 0 && (maze[offset(-1, 0)] & VISITED) == 0)
					neighbours.push_back(3);

				if (!neighbours.empty())
				{
					int nextDirection = neighbours[rand() % neighbours.size()];

					switch (nextDirection)
					{
					case 0://Up
						maze[offset(0, 0)] |= UP;
						maze[offset(0, -1)] |= VISITED | DOWN;
						blocks.push(make_pair((blocks.top().first + 0), (blocks.top().second - 1)));
						break;
					case 1://Right
						maze[offset(0, 0)] |= RIGHT;
						maze[offset(1, 0)] |= VISITED | LEFT;
						blocks.push(make_pair((blocks.top().first + 1), (blocks.top().second + 0)));
						break;
					case 2://Down
						maze[offset(0, 0)] |= DOWN;
						maze[offset(0, 1)] |= VISITED | UP;
						blocks.push(make_pair((blocks.top().first + 0), (blocks.top().second + 1)));
						break;
					case 3://Left
						maze[offset(0, 0)] |= LEFT;
						maze[offset(-1, 0)] |= VISITED | RIGHT;
						blocks.push(make_pair((blocks.top().first - 1), (blocks.top().second + 0)));
						break;

					}
					visitedCount++;
				}
				else
				{
					blocks.pop();
				}


			}
		}

		void drawRBMaze()
		{
			//Clear Screen
			Fill(0, 0, windowWidth, windowHeight, L' ');
			//Iterate through the maze and draw the blocks
			for (int x = 0; x < mazeWidth; x++)
			{
				for (int y = 0; y < mazeHeight; y++)
				{
					//Each block is 3x3 and the lines(walls) are 1 pixel wide
					for (int j = 0; j < pathWidth; j++)
						for (int i = 0; i < pathWidth; i++)
						{
							if (maze[y*mazeWidth + x] & VISITED)
								Draw(x * (pathWidth + 1) + i, y* (pathWidth + 1) + j, PIXEL_SOLID, FG_WHITE);
							else
								Draw(x* (pathWidth + 1) + i, y* (pathWidth + 1) + j, PIXEL_SOLID, FG_RED);
						}
					for (int p = 0; p < pathWidth; p++)
					{
						if (maze[y * mazeWidth + x] & DOWN)
							Draw(x * (pathWidth + 1) + p, y * (pathWidth + 1) + pathWidth);

						if (maze[y * mazeWidth + x] & RIGHT)
							Draw(x * (pathWidth + 1) + pathWidth, y * (pathWidth + 1) + p);
					}
				}
			}

			// Draw the current block
			for (int py = 0; py < pathWidth; py++)
				for (int px = 0; px < pathWidth; px++)
					Draw(blocks.top().first * (pathWidth + 1) + px, blocks.top().second * (pathWidth + 1) + py, 0x2588, FG_CYAN);
		}
};

