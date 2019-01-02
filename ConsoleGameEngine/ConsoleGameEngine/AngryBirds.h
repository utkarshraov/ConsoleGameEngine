#pragma once
#include "ConsoleGameEngine.h"
#include <vector>
using namespace std;

class Obstacle
{
public:
	Vector2 position;
	int scale;
	int colour;
	int hardness;
};

class AngryBirds : public ConsoleGameEngine
{
public:
	AngryBirds()
	{
		appName = L'Angry Birds';
	}
	virtual bool Start();
	virtual bool Update(float);
private:
	vector<Obstacle> obstacles;
};

