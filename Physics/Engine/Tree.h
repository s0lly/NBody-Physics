#pragma once


/*******************************************************************************************
*	Code and amendments by s0lly														   *
*	https://www.youtube.com/c/s0lly							                               *
*	https://s0lly.itch.io/																   *
*	https://www.instagram.com/s0lly.gaming/												   *
********************************************************************************************/


#include "Vec2.h"
#include "WorldObject.h"
#include <thread>

struct Tree
{
	Vec2 *nodeAveLoc;
	float *nodeTotalMass;
	std::vector<std::vector<int>> nodeObjectsContained;

	int totalNodes = 0;
	int numPlanes = 0;

	Vec2 topLeft;
	Vec2 botRight;

	float treeWidth;
	float treeHeight;



	void Init(int currentAssignedObjects)
	{
		topLeft = Vec2();
		botRight = Vec2();

		int lowestPowerOf4 = 1;
		numPlanes = 0;

		while (lowestPowerOf4 < currentAssignedObjects)
		{
			lowestPowerOf4 *= 4;
			numPlanes++;
		}
		lowestPowerOf4 *= 4;
		numPlanes++;
		lowestPowerOf4 *= 4;
		numPlanes++;

		totalNodes = 0;

		for (int p = 0; p < numPlanes; p++)
		{
			totalNodes += pow(4, p);
		}

		nodeAveLoc = new Vec2[totalNodes]();
		nodeTotalMass = new float[totalNodes]();
		nodeObjectsContained.clear();
		nodeObjectsContained = std::vector<std::vector<int>>(pow(4, numPlanes));
	}



	void Refresh(WorldObjects &worldObjects)
	{
		// Worldspace is the highest level space (equivalent to maxPlane). Each space has 4 qaudrants.
		// set worldObjects to detailed plane nodes

		topLeft = Vec2();
		botRight = Vec2();

		for (int i = 0; i < worldObjects.currentAssignedObjects; i++)
		{
			topLeft.x = topLeft.x < worldObjects.loc[i].x ? topLeft.x : worldObjects.loc[i].x - 1.0f;
			topLeft.y = topLeft.y < worldObjects.loc[i].y ? topLeft.y : worldObjects.loc[i].y - 1.0f;
			botRight.x = botRight.x > worldObjects.loc[i].x ? botRight.x : worldObjects.loc[i].x + 1.0f;
			botRight.y = botRight.y > worldObjects.loc[i].y ? botRight.y : worldObjects.loc[i].y + 1.0f;
		}

		treeWidth = botRight.x - topLeft.x;
		treeHeight = botRight.y - topLeft.y;

		// We want some higher power of 4 than current worldObjects to get fine resolution on calculations

		memset(nodeAveLoc, 0, sizeof(Vec2) * totalNodes);
		memset(nodeTotalMass, 0, sizeof(float) * totalNodes);
		nodeObjectsContained = std::vector<std::vector<int>>(pow(4, numPlanes));

		auto worldObjectsPtr = &worldObjects;
		auto treePtr = this;

		int numThreads = 4;

		int detailedNodeDim = pow(2, (numPlanes - 1));
		float treeNodeWidthInv = 1.0f / (treeWidth / (float)detailedNodeDim);
		float treeNodeHeightInv = 1.0f / (treeHeight / (float)detailedNodeDim);

		std::vector<std::thread> threadList;
		for (int k = 0; k < numThreads; k++)
		{
			threadList.push_back(std::thread([treeNodeWidthInv, treeNodeHeightInv, detailedNodeDim, worldObjectsPtr, k, treePtr]()
			{
				for (int i = 0; i < worldObjectsPtr->currentAssignedObjects; i++)
				{
					int x = (int)((worldObjectsPtr->loc[i].x - treePtr->topLeft.x) * treeNodeWidthInv);
					int y = (int)((worldObjectsPtr->loc[i].y - treePtr->topLeft.y) * treeNodeHeightInv);

					if (x >= (detailedNodeDim * (k % 2)) / 2 && x < (detailedNodeDim * ((k % 2) + 1)) / 2
						&& y >= (detailedNodeDim * (int)(k / 2)) / 2 && y < (detailedNodeDim * ((int)(k / 2) + 1)) / 2)
					{
						int detailedNode = y * detailedNodeDim + x;

						worldObjectsPtr->nodeID[i] = detailedNode;
						(*treePtr).nodeObjectsContained[detailedNode].push_back(i);


						treePtr->nodeAveLoc[detailedNode] = treePtr->nodeAveLoc[detailedNode] + worldObjectsPtr->loc[i] * worldObjectsPtr->mass[i];
						treePtr->nodeTotalMass[detailedNode] = treePtr->nodeTotalMass[detailedNode] + worldObjectsPtr->mass[i];

						// we've loaded in lowest plane, now work up

						int currX = x;
						int currY = y;
						int numOfPriorPlaneNodes = 0;

						Vec2 locTimesMass = worldObjectsPtr->loc[i] * worldObjectsPtr->mass[i];

						for (int p = 1; p < treePtr->numPlanes - 1; p++)
						{
							numOfPriorPlaneNodes += pow(4, (treePtr->numPlanes - p));

							currX /= 2;
							currY /= 2;

							int planeNode = numOfPriorPlaneNodes + currY * pow(2, (treePtr->numPlanes - p - 1)) + currX;

							treePtr->nodeAveLoc[planeNode] = treePtr->nodeAveLoc[planeNode] + locTimesMass;
							treePtr->nodeTotalMass[planeNode] = treePtr->nodeTotalMass[planeNode] + worldObjectsPtr->mass[i];
						}
					}
				}
			}));
		}
		std::for_each(threadList.begin(), threadList.end(), std::mem_fn(&std::thread::join));



		for (int n = 0; n < totalNodes - 1; n++)
		{
			if (nodeTotalMass[n] > 0.0f)
			{
				nodeAveLoc[n] = nodeAveLoc[n] / nodeTotalMass[n];
			}
		}

	}
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
								ApplyGravityToFirst(&worldObjects->loc[worldObjNum], &worldObjects->mass[worldObjNum], &worldObjects->velocity[worldObjNum],
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
					if (!ApplyGravityToFirst(&worldObjects->loc[worldObjNum], &worldObjects->mass[worldObjNum], &worldObjects->velocity[worldObjNum],
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
								ApplyGravityToFirst(&worldObjects->loc[worldObjNum], &worldObjects->mass[worldObjNum], &worldObjects->velocity[worldObjNum],
									worldObjects->loc[targetWorldObject], worldObjects->mass[targetWorldObject], dt, false);
							}
						}
					}
				}
			}
				
		}
	}


};