#include "AngryBirds.h"

bool AngryBirds::Start()
{
	//initialise and randomly place obstacles
	for (int i = 0; i < 10; i++)
	{
		Obstacle obs;
		obs.position = Vector2((rand() % (windowWidth / 2)) * 2, rand() % windowHeight);
		obs.scale = rand() % 5;
		obs.hardness = rand() % 3;
		if (obs.hardness == 0)
			obs.colour = FG_GREEN;
		else if (obs.hardness == 1)
			obs.colour = FG_YELLOW;
		else if (obs.hardness == 2)
			obs.colour = FG_RED;
		obstacles.push_back(obs);
	}
	
	return true;
}

bool AngryBirds::Update(float)
{
	
	Fill(0, 0, windowWidth, windowHeight, ' ');
	for (auto obs : obstacles)
	{
		DrawCircle(obs.position.x, obs.position.y, obs.scale, PIXEL_SOLID, obs.colour);
	}
	
	return true;
}
