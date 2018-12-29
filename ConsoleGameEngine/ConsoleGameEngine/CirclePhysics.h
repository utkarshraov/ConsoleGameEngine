#pragma once
#include "ConsoleGameEngine.h"
using namespace std;
const float PI  = 3.14159;
struct bouncyBall
{
	Vector2 position;
	Vector2 velocity;
	Vector2 acceleration;
	float radius;
	float mass;
	int identifier;
};


class CirclePhysics : public ConsoleGameEngine
{
public:
	CirclePhysics() 
	{
		appName = L'Bouncy Balls';
	}
private:
	vector<pair<float, float>> circleModel;

	vector<bouncyBall> balls;

	bouncyBall * selectedBall = nullptr;
	void createBall(float x, float y, float r = 5.0f)
	{
		bouncyBall b;
		b.position = Vector2(x,y);
		b.velocity = Vector2();
		b.acceleration - Vector2();
		b.radius = r;
		b.identifier = balls.size();
		b.mass = r * 5.0f;
		balls.push_back(b);
	}

public:

	virtual bool Start()
	{
		circleModel.push_back({ 0.0f, 0.0f });
		int points = 20;
		for (int i = 0; i < points; i++)
			circleModel.push_back({ cosf(i / (float)(points - 1) * 2.0f * PI), sinf(i / (float)(points - 1) * 2.0f * PI) });
		//createBall(windowWidth * 0.25f, windowHeight * 0.5f, 5.0f);
		//createBall(windowWidth * 0.5f, windowHeight * 0.5f, 5.0f);
		for(int i=0;i<10;i++)
			createBall(rand()%windowWidth, rand()%windowHeight, rand()%16 + 4);

		return true;
	}
	virtual bool Update(float elapsedTime)
	{
		auto doCirclesOverlap = [](float x1, float y1, float r1, float x2, float y2, float r2)
		{
			return (fabs((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2)) <= (r1 + r2) * (r1 + r2));
		};

		auto isPointInCircle = [](float x1, float y1, float r1, float posx, float posy)
		{
			return fabs((x1 - posx)*(x1 - posx) + (y1 - posy)*(y1 - posy)) < (r1*r1);
		};
		
		//allow mouse click to select balls to move
		if (mouse[0].onPressed || mouse[1].onPressed)
		{
			selectedBall = nullptr; //deselect first
			for (auto &ball : balls)
			{
				//if mousePosition is on a ball, select that ball
				if (isPointInCircle(ball.position.x, ball.position.y, ball.radius, mousePosX, mousePosY))
				{
					selectedBall = &ball;
					break;
				}
			}
		}

		if (mouse[0].onHeld)
		{
			if (selectedBall != nullptr)
			{
				selectedBall->position = Vector2(mousePosX, mousePosY);
			}
		}

		if (mouse[0].onReleased)
		{
			selectedBall = nullptr;
		}

		if (mouse[1].onReleased)
		{
			if (selectedBall != nullptr)
			{
				selectedBall->velocity = ((selectedBall->position - Vector2(mousePosX, mousePosY))) * 5.0f;
			}
			selectedBall = nullptr;
		}

		for (auto &ball : balls)
		{
			ball.acceleration = ball.velocity * -0.7f; //drag
			
			//move balls based on physics
			ball.velocity = ball.velocity + ball.acceleration * elapsedTime;
			ball.position = ball.position + ball.velocity * elapsedTime;

			//wrap the balls around the screen if they go off the edge
			if (ball.position.x < 0) ball.position.x += (float)windowWidth;
			if (ball.position.y < 0) ball.position.y += (float)windowHeight;
			if (ball.position.x >= windowWidth) ball.position.x -= (float)windowWidth;
			if (ball.position.y >= windowHeight) ball.position.y -= (float)windowHeight;

			if (ball.velocity.magnitude() < 0.1f)
			{
				ball.velocity = Vector2(0, 0);
			}
		}
		
		vector<pair<bouncyBall *, bouncyBall*>> collidingPairs;
		//keep track of colliding pairs for visual indicator
	
		//check if any spheres are overlapping
		for (auto &ball : balls)
		{
			for (auto &target : balls)
			{
				if (ball.identifier != target.identifier)
				{
					if (doCirclesOverlap(ball.position.x, ball.position.y, ball.radius, target.position.x, target.position.y, target.radius))
					{
						collidingPairs.push_back({ &ball,&target });
						//these two spheres overlap
						float distance = (ball.position - target.position).magnitude();

						float overlap = 0.5f * (distance - ball.radius - target.radius);

						//displace them away from each other (static collision response)
						ball.position = ball.position - ((ball.position - target.position) * overlap).normalise();
						target.position = target.position + ((ball.position - target.position) * overlap).normalise();


					}
				}
			}
		}

		for (auto coll : collidingPairs)
		{
			//dynamic collision response
			bouncyBall * b1 = coll.first;
			bouncyBall * b2 = coll.second;

			float distance = (b1->position - b2->position).magnitude();

			Vector2 normal = (b2->position - b1->position).normalise();

			Vector2 tangent = Vector2(-normal.y, normal.x);

			//tangential response
			float tangentDot1 = dot(b1->velocity, tangent);
			float tangentDot2 = dot(b2->velocity, tangent);

			//normal response
			float normalDot1 = dot(b1->velocity, normal);
			float normalDot2 = dot(b2->velocity, normal);

			//calculating for conservation of momentum (wikipedia)
			float mom1 = (normalDot1 * (b1->mass - b2->mass) + 2.0f * b2->mass * normalDot2) / (b1->mass + b2->mass);
			float mom2 = (normalDot2 * (b2->mass - b1->mass) + 2.0f * b1->mass * normalDot1) / (b1->mass + b2->mass);

			//update ball velocities
			b1->velocity = tangent * tangentDot1 + normal * mom1;
			b2->velocity = tangent * tangentDot2 + normal * mom2;
		}


		Fill(0, 0, windowWidth, windowHeight, ' ');

		for(auto ball:balls)
			DrawCircle(ball.position.x, ball.position.y, ball.radius, PIXEL_SOLID, FG_WHITE);


		for (auto coll : collidingPairs)
		{
			//draw red line between colliding spheres so it looks cool
			DrawLine(coll.first->position.x, coll.first->position.y, coll.second->position.x, coll.second->position.y, PIXEL_SOLID, FG_RED);
		}
		
		if (selectedBall != nullptr)
		{
			DrawLine(selectedBall->position.x, selectedBall->position.y, mousePosX, mousePosY, PIXEL_SOLID, FG_DARK_BLUE);
		}
		
		return true;
	}
};

