#pragma once


/*******************************************************************************************
*	Code and amendments by s0lly														   *
*	https://www.youtube.com/c/s0lly							                               *
*	https://s0lly.itch.io/																   *
*	https://www.instagram.com/s0lly.gaming/												   *
********************************************************************************************/


#include "Vec2.h"
#include "WorldObject.h"

struct TreeNode
{
	Vec2 averageLoc;
	float totalMass;
};

struct Tree
{
	Vec2 topLeft;
	Vec2 botRight;
};


static void
RecursivePlaneQuadrantCheckAndApplyGravity(WorldObjects *worldObjects, int maxWorldObjects, int worldObjNum, int maxPlanes, int priorPlane,
	Vec2 *nodeAveLocs, float *nodeTotalMasses, std::vector<std::vector<int>> * nodeObjectsContained, int priorX, int priorY, float dt)
{
	int priorPlaneNodeStart = 0;
	int quadrantPlaneNodeStart = 0;

	for (int p = maxPlanes - priorPlane; p < maxPlanes; p++)
	{
		priorPlaneNodeStart += pow(4, p);
	}
	for (int p = maxPlanes - priorPlane + 1; p < maxPlanes; p++)
	{
		quadrantPlaneNodeStart += pow(4, p);
	}

	int newXStart = priorX * 2;
	int newYStart = priorY * 2;
	int newNodeDim = pow(2, maxPlanes - priorPlane);

	int worldObjNodeIDDim = pow(2, maxPlanes - 1);
	int worldObjNodeIDX = worldObjects->nodeID[worldObjNum] % worldObjNodeIDDim;
	int worldObjNodeIDY = worldObjects->nodeID[worldObjNum] / worldObjNodeIDDim;

	worldObjNodeIDX = worldObjNodeIDX / (pow(2, priorPlane - 1));
	worldObjNodeIDY = worldObjNodeIDY / (pow(2, priorPlane - 1));

	for (int y = newYStart; y < newYStart + 2; y++)
	{
		for (int x = newXStart; x < newXStart + 2; x++)
		{
			int nodeLookup = quadrantPlaneNodeStart + y * newNodeDim + x;

			bool isObjInNode = (worldObjNodeIDX == x && worldObjNodeIDY == y);

			bool atLowestPlaneLevel = (priorPlane == 1);

			if (nodeTotalMasses[nodeLookup] > 0.0f)
			{
				if (isObjInNode)
				{
					if (atLowestPlaneLevel)
					{
						for (int t = 0; t < (*nodeObjectsContained)[nodeLookup].size(); t++)
						{
							int targetWorldObject = (*nodeObjectsContained)[nodeLookup][t];
							if (worldObjNum != targetWorldObject)
							{
								ApplyGravityToFirst(&worldObjects->loc[worldObjNum], &worldObjects->mass[worldObjNum], &worldObjects->velocity[worldObjNum], &worldObjects->calcsCompleted[worldObjNum],
									worldObjects->loc[targetWorldObject], worldObjects->mass[targetWorldObject], dt, false);
							}
						}
					}
					else
					{
						RecursivePlaneQuadrantCheckAndApplyGravity(worldObjects, maxWorldObjects, worldObjNum, maxPlanes, priorPlane - 1,
							nodeAveLocs, nodeTotalMasses, nodeObjectsContained, x, y, dt);
					}
				}
				else
				{
					if (!ApplyGravityToFirst(&worldObjects->loc[worldObjNum], &worldObjects->mass[worldObjNum], &worldObjects->velocity[worldObjNum], &worldObjects->calcsCompleted[worldObjNum],
						nodeAveLocs[nodeLookup], nodeTotalMasses[nodeLookup], dt, true))
					{
						if (!atLowestPlaneLevel)
						{
							RecursivePlaneQuadrantCheckAndApplyGravity(worldObjects, maxWorldObjects, worldObjNum, maxPlanes, priorPlane - 1,
								nodeAveLocs, nodeTotalMasses, nodeObjectsContained, x, y, dt);
						}
						else
						{
							for (int t = 0; t < (*nodeObjectsContained)[nodeLookup].size(); t++)
							{
								int targetWorldObject = (*nodeObjectsContained)[nodeLookup][t];
								ApplyGravityToFirst(&worldObjects->loc[worldObjNum], &worldObjects->mass[worldObjNum], &worldObjects->velocity[worldObjNum], &worldObjects->calcsCompleted[worldObjNum],
									worldObjects->loc[targetWorldObject], worldObjects->mass[targetWorldObject], dt, false);
							}
						}
					}
				}
			}
				
		}
	}


};