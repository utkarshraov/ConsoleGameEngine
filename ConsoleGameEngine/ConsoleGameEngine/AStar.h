#pragma once
#include "ConsoleGameEngine.h"
#include <vector>
using namespace std;

class AStar : public ConsoleGameEngine
{
private:
	struct Node
	{
		//Nodes of the graphs
		bool isObstacle = false;
		bool isVisited = false;
		float globalDistance;
		float localDistance;
		int x;
		int y;
		vector<Node*> neighbours;
		Node * parent;
	};

	Node *nodes = nullptr;
	int mapWidth = 16;
	int mapHeight = 16;

	Node *startNode = nullptr;
	Node * endNode = nullptr;

public:
	AStar()
	{

	}
protected:
	virtual bool Start()
	{
		//instantiate and setup the grid of nodes
		nodes = new Node[mapWidth * mapHeight];
		for (int x = 0; x < mapWidth; x++)
		{
			for (int y = 0; y < mapHeight; y++)
			{
				nodes[y*mapWidth + x].x = x;
				nodes[y*mapWidth + x].y = y;
				nodes[y*mapWidth + x].isObstacle = false;
				nodes[y*mapWidth + x].parent = nullptr;
				nodes[y*mapWidth + x].isVisited = false;

			}
		}

		startNode = &nodes[(mapHeight / 2) * mapWidth + 1];
		endNode = &nodes[(mapHeight / 2) * mapWidth - 2];

		//create connections between "connected" nodes
		for (int x = 0; x < mapWidth; x++)
		{
			for (int y = 0; y < mapHeight; y++)
			{
				if(y>0)
					nodes[y*mapWidth + x].neighbours.push_back(&nodes[(y - 1) * mapWidth + (x + 0)]);
				if(y<mapHeight - 1)
					nodes[y*mapWidth + x].neighbours.push_back(&nodes[(y + 1) * mapWidth + (x + 0)]);
				if (x > 0)
					nodes[y*mapWidth + x].neighbours.push_back(&nodes[(y - 0) * mapWidth + (x - 1)]);
				if (x < mapWidth - 1)
					nodes[y*mapWidth + x].neighbours.push_back(&nodes[(y + 0) * mapWidth + (x + 1)]);

				// diagonal connections	
				if (y>0 && x>0)
					nodes[y*mapWidth + x].neighbours.push_back(&nodes[(y - 1) * mapWidth + (x - 1)]);
				if (y<mapHeight-1 && x>0)
					nodes[y*mapWidth + x].neighbours.push_back(&nodes[(y + 1) * mapWidth + (x - 1)]);
				if (y>0 && x< mapWidth -1)
					nodes[y*mapWidth + x].neighbours.push_back(&nodes[(y - 1) * mapWidth + (x + 1)]);
				if (y< mapHeight - 1 && x< mapWidth -1)
					nodes[y*mapWidth + x].neighbours.push_back(&nodes[(y + 1) * mapWidth + (x + 1)]);
				
			}
		}
		solveAStar();
		return true;

	}

	bool solveAStar()
	{
		auto distance = [](Node* a, Node* b)
		{
			return sqrtf((a->x - b->x)*(a->x - b->x) + (a->y - b->y)*(a->y - b->y));
		};

		auto ASHeuristic = [distance](Node* a, Node * b)
		{	
			return distance(a, b);
		};
		
		//Reset NavGraph
		for (int x = 0; x < mapWidth; x++)
		{
			for (int y = 0; y < mapHeight; y++)
			{
				nodes[y*mapWidth + x].parent = nullptr;
				nodes[y*mapWidth + x].isVisited = false;
				nodes[y*mapWidth + x].globalDistance = INFINITY;
				nodes[y*mapWidth + x].localDistance = INFINITY;
			}
		}

		Node *currentNode = startNode;
		startNode->localDistance = 0.0f;
		startNode->globalDistance = ASHeuristic(startNode, endNode);
		
		list<Node*> notTestedNodes;
		notTestedNodes.push_back(startNode);

		while (!notTestedNodes.empty())
		{
			notTestedNodes.sort([](const Node* lhs, const Node* rhs) { return lhs->globalDistance < rhs->globalDistance; });

			while (!notTestedNodes.empty() && notTestedNodes.front()->isVisited)
				notTestedNodes.pop_front();

			if (notTestedNodes.empty())
				break;

			currentNode = notTestedNodes.front();
			currentNode->isVisited = true;

			for (auto neighbour : currentNode->neighbours)
			{
				if (!neighbour->isVisited && neighbour->isObstacle == 0)
					notTestedNodes.push_back(neighbour);

				float potentialLowerDistance = currentNode->localDistance + distance(currentNode, neighbour);

				if (potentialLowerDistance < neighbour->localDistance)
				{
					neighbour->parent = currentNode;
					neighbour->localDistance = potentialLowerDistance;

					neighbour->globalDistance = neighbour->localDistance + ASHeuristic(neighbour, endNode);
				}
			}
		}
		return true;
	}

	virtual bool Update(float elapsedTime)
	{
		int nodeSize = 9;
		int borderSize = 2;

		int selectedNodeX = mousePosX / nodeSize;
		int selectedNodeY = mousePosY / nodeSize;

		//Create Obstacles with a left click on a node
		if (mouse[0].onReleased)
		{
			if(selectedNodeX >=0 && selectedNodeX < mapWidth)
				if (selectedNodeY >= 0 && selectedNodeY < mapHeight)
				{
					if (keys[VK_SHIFT].onHeld)
						startNode = &nodes[selectedNodeY * mapWidth + selectedNodeX];
					else if (keys[VK_CONTROL].onHeld)
						endNode = &nodes[selectedNodeY * mapWidth + selectedNodeX];
					else
						nodes[selectedNodeY * mapWidth + selectedNodeX].isObstacle = !nodes[selectedNodeY * mapWidth + selectedNodeX].isObstacle;
				}
			solveAStar();
		}

		Fill(0, 0, windowWidth, windowHeight, L' ');

		for (int x = 0; x < mapWidth; x++)
		{
			for (int y = 0; y < mapHeight; y++)
			{
				for (auto n : nodes[y*mapWidth + x].neighbours)
				{
					DrawLine(x * nodeSize + nodeSize / 2, y*nodeSize + nodeSize / 2, n->x *nodeSize + nodeSize / 2, n->y*nodeSize + nodeSize / 2, PIXEL_SOLID, FG_DARK_GREEN);
				}
			}
		}

		for (int x = 0; x < mapWidth; x++)
		{
			for (int y = 0; y < mapHeight; y++)
			{
				Fill(x *nodeSize + borderSize, y*nodeSize + borderSize, (x + 1) * nodeSize - borderSize, (y + 1) * nodeSize - borderSize, PIXEL_HALF, nodes[y*mapWidth + x].isObstacle ? FG_DARK_RED : FG_BLUE);
				if (&nodes[y* mapWidth + x].isVisited)
					Fill(x *nodeSize + borderSize, y*nodeSize + borderSize, (x + 1) * nodeSize - borderSize, (y + 1) * nodeSize - borderSize, PIXEL_SOLID, nodes[y*mapWidth + x].isObstacle ? FG_DARK_RED : FG_BLUE);
				if(&nodes[y* mapWidth + x] == startNode)
					Fill(x *nodeSize + borderSize, y*nodeSize + borderSize, (x + 1) * nodeSize - borderSize, (y + 1) * nodeSize - borderSize, PIXEL_THREEQUARTERS, FG_YELLOW);
				if(&nodes[y* mapWidth + x] == endNode)
					Fill(x *nodeSize + borderSize, y*nodeSize + borderSize, (x + 1) * nodeSize - borderSize, (y + 1) * nodeSize - borderSize, PIXEL_THREEQUARTERS, FG_GREEN);
			}
		}

		if (endNode != nullptr)
		{
			Node * p = endNode;
			while (p->parent != nullptr)
			{
				
				DrawLine(p->x * nodeSize + nodeSize / 2, p->y*nodeSize + nodeSize / 2, p->parent->x *nodeSize + nodeSize / 2, p->parent->y*nodeSize + nodeSize / 2, PIXEL_SOLID, FG_YELLOW);
				p = p->parent;
			}
		}

		
		return true;
	}
};