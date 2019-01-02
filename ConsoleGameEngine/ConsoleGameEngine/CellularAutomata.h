#pragma once
#include "ConsoleGameEngine.h"
using namespace std;

class CellularAutomata : public ConsoleGameEngine
{

public:
	CellularAutomata()
	{
		appName = L'CellularAutomata';
	}
private:
	int *output;
	int *state;

	virtual bool Start()
	{
		output = new int[windowWidth * windowHeight];
		state = new int[windowWidth * windowHeight];
		memset(output, 0x00, windowWidth * windowHeight);
		memset(state, 0x00, windowWidth * windowHeight);

		for (int i = 0; i < windowWidth * windowHeight; i++)
			state[i] = rand() % 2;
		
		return true;
	}

	virtual bool Update(float elapsedTime)
	{
		
		this_thread::sleep_for(5ms);
		auto cell = [&](int x, int y)
		{
			return output[y* windowWidth + x];
		};

		for (int i = 0; i < windowWidth * windowHeight; i++)
			output[i] = state[i];

		//ignore boundaries to keep it from breaking
		for (int x = 1; x < windowWidth - 1; x++)
		{
			for (int y = 1; y < windowHeight - 1; y++)
			{
				int neighbours = cell(x - 1, y - 1) + cell(x + 0, y - 1) + cell(x + 1, y - 1) +
					cell(x - 1, y + 0) + 0 + cell(x + 1, y + 0) +
					cell(x - 1, y + 1) + cell(x + 0, y + 1) + cell(x + 1, y + 1);

				if (cell(x, y) == 1)
					state[y*windowWidth + x] = neighbours == 2 || neighbours == 3;
				else
					state[y*windowWidth + x] = neighbours == 3;

				if (cell(x, y) == 1)
					Draw(x, y, PIXEL_SOLID, rand()%2 + 12);
				else
					Draw(x, y, PIXEL_SOLID, FG_BLACK);
			}
		}
		
		return true;
	}
};