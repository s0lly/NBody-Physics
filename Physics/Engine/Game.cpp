/****************************************************************************************** 
 *	Chili DirectX Framework Version 16.07.20											  *	
 *	Game.cpp																			  *
 *	Copyright 2016 PlanetChili.net <http://www.planetchili.net>							  *
 *																						  *
 *	This file is part of The Chili DirectX Framework.									  *
 *																						  *
 *	The Chili DirectX Framework is free software: you can redistribute it and/or modify	  *
 *	it under the terms of the GNU General Public License as published by				  *
 *	the Free Software Foundation, either version 3 of the License, or					  *
 *	(at your option) any later version.													  *
 *																						  *
 *	The Chili DirectX Framework is distributed in the hope that it will be useful,		  *
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of						  *
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the						  *
 *	GNU General Public License for more details.										  *
 *																						  *
 *	You should have received a copy of the GNU General Public License					  *
 *	along with The Chili DirectX Framework.  If not, see <http://www.gnu.org/licenses/>.  *
 ******************************************************************************************/

 /*******************************************************************************************
 *	Code and amendments by s0lly														   *
 *	https://www.youtube.com/c/s0lly							                               *
 *	https://s0lly.itch.io/																   *
 *	https://www.instagram.com/s0lly.gaming/												   *
 ********************************************************************************************/





#include "MainWindow.h"
#include "Game.h"

Game::Game(MainWindow& wnd)
	:
	wnd(wnd),
	gfx(wnd)
{
	// Initialise objects

	worldObjects.Init(numObjects);



	tree.Init(worldObjects.currentAssignedObjects);

	cameraLoc = Vec2(0.0f, 0.0f);

	start = std::chrono::system_clock::now(); // 500ms
}

void Game::Go()
{


	gfx.BeginFrame();

	ProcessInput();
	UpdateModel();
	ComposeFrame();

	gfx.EndFrame();

	

	

	
}

void Game::ProcessInput()
{
	// Camera movements - arrow keys are effectively disabled as camera is locked on to largest object

	if (wnd.kbd.KeyIsPressed(VK_UP))
	{
		cameraLoc.y += 5.0f;
	}
	if (wnd.kbd.KeyIsPressed(VK_DOWN))
	{
		cameraLoc.y -= 5.0f;
	}
	if (wnd.kbd.KeyIsPressed(VK_LEFT))
	{
		cameraLoc.x -= 5.0f;
	}
	if (wnd.kbd.KeyIsPressed(VK_RIGHT))
	{
		cameraLoc.x += 5.0f;
	}

	if (wnd.kbd.KeyIsPressed('A'))
	{
		cameraZoomOut *= 0.9f;
	}
	if (wnd.kbd.KeyIsPressed('Z'))
	{
		cameraZoomOut *= 1.1f;
	}
}

void Game::UpdateModel()
{
	
	tree.Refresh(worldObjects);






	
	

	// We now have a pyramid of nodes

	
	//////////////////////
	//
	// Algorithm approach:
	//
	// for each object:
	// we check the first level of quadrants
	//
	// if the quadrant has a total mass vs distance at some ratio, check its quadrants for further examination
	// or if the quadrant contains the particle itself, check its quadrants
	//		if the quadrant is at the highest level of resolution, use that quadrant's gravity
	//
	// else, apply that quadrant's gravity to the object in question
	//
	// lastly, for all objects in the quadrant of the object itself (if any), process each object individually
	//
	//////////////////////

	

	// do a memcopy here?
	for (int i = 0; i < worldObjects.currentAssignedObjects; i++)
	{
		worldObjects.oldVelocity[i] = worldObjects.velocity[i];
	}

	
	
	// we will want to check the topmost quadrant first
	int currentPlane = tree.numPlanes - 1;

	numCalcs = 0.0f;

	auto worldObjectsPtr = &worldObjects;
	auto treePtr = &tree;
	auto dtPtr = &dt;

	int numThreads2 = 20;
	int threadSize = worldObjects.currentAssignedObjects / numThreads2 + 1;

	std::vector<std::thread> threadList2;
	for (int k = 0; k < numThreads2; k++)
	{
		threadList2.push_back(std::thread([worldObjectsPtr, k, dtPtr, threadSize, treePtr]()
		{
			for (int i = k * threadSize; (i < (k + 1) * threadSize) && (i < (worldObjectsPtr->currentAssignedObjects)); i++)
			{
				//worldObjectsPtr->calcsCompleted[i] = 0;

				RecursivePlaneQuadrantCheckAndApplyGravity(worldObjectsPtr, (worldObjectsPtr->currentAssignedObjects), i, treePtr->numPlanes, treePtr->numPlanes - 1,
					treePtr->nodeAveLoc, treePtr->nodeTotalMass, &(treePtr->nodeObjectsContained), 0, 0, (*dtPtr));
			}
		}));
	}
	std::for_each(threadList2.begin(), threadList2.end(), std::mem_fn(&std::thread::join));


	

	//for (int i = 0; i < currentAssignedObjects; i++)
	//{
	//	numCalcs += (float)worldObjects.calcsCompleted[i];
	//}



	// Update all objects locations based on all active forces
	for (int i = 0; i < worldObjects.currentAssignedObjects; i++)
	{
		worldObjects.loc[i] = worldObjects.loc[i] +  (worldObjects.oldVelocity[i] + worldObjects.velocity[i]) * dt * 0.5f;
	}

	
	optimiseStart = std::chrono::system_clock::now(); // 145ms

	// Merge objects that have collided
	//if (worldObjects.currentAssignedObjects > 1)
	//{
	//	for (int i = 0; i < worldObjects.currentAssignedObjects - 1; i++)
	//	{
	//		for (int j = i + 1; j < worldObjects.currentAssignedObjects; j++)
	//		{
	//			if (CheckCollision(worldObjects, i, j))
	//			{
	//				MergeObjects(&worldObjects, i, j);
	//				j--;
	//				i--;
	//				j--;
	//				break;
	//			}
	//		}
	//	}
	//}

	optimiseEnd = std::chrono::system_clock::now();
	std::chrono::duration<float> optimisedElapsedTime = optimiseEnd - optimiseStart;
	optimiseDt = optimisedElapsedTime.count(); //dt = 1.0f;//



	// Lock camera on to largest object
	//
	//float alignedMass = 0.0f;
	//Vec2 newLoc = cameraLoc;
	//for (int i = 0; i < currentAssignedObjects; i++)
	//{
	//	if (alignedMass < worldObjects[i].mass) 
	//	{
	//		alignedMass = worldObjects[i].mass;
	//		newLoc = worldObjects[i].loc;
	//	}
	//}


	// Lock camera on to center of total mass
	
	float alignedMassXLoc = 0.0f;
	float alignedMassYLoc = 0.0f;
	float totalMass = 0.0f;
	
	for (int i = 0; i < worldObjects.currentAssignedObjects; i++)
	{
		alignedMassXLoc += worldObjects.loc[i].x * worldObjects.mass[i];
		alignedMassYLoc += worldObjects.loc[i].y * worldObjects.mass[i];
		totalMass += worldObjects.mass[i];
	}
	alignedMassXLoc /= totalMass;
	alignedMassYLoc /= totalMass;
	
	Vec2 newLoc = Vec2(alignedMassXLoc, alignedMassYLoc);



	// Smooth camera movement
	
	float percentageToMove = 0.1f;
	cameraLoc = Vec2((newLoc.x * percentageToMove + cameraLoc.x * (1.0f - percentageToMove)), (newLoc.y * percentageToMove + cameraLoc.y * (1.0f - percentageToMove)));



	// Smooth camera zoom

	//float smallestY = 0.0f;
	//float largestY = 1.0f;
	//
	//for (int i = 0; i < currentAssignedObjects; i++)
	//{
	//	if (worldObjects.loc[i].y - cameraLoc.y > largestY)
	//	{
	//		largestY = worldObjects.loc[i].y - cameraLoc.y;
	//	}
	//	if (cameraLoc.y - worldObjects.loc[i].y > smallestY)
	//	{
	//		smallestY = cameraLoc.y - worldObjects.loc[i].y;
	//	}
	//}
	//
	//float newY = largestY > smallestY ? largestY : smallestY;
	//
	//float newCameraZoomOut = ((2.0f * newY) / (float)gfx.ScreenHeight) * 1.2f;
	//
	//percentageToMove = 0.02f;
	//cameraZoomOut = newCameraZoomOut * percentageToMove + cameraZoomOut * (1.0f - percentageToMove);

}

void Game::ComposeFrame()
{
	
	// Draw all objects

	for (int i = 0; i < worldObjects.currentAssignedObjects; i++)
	{
		gfx.DrawCircle(Vec2((worldObjects.loc[i].x - cameraLoc.x) / (cameraZoomOut)+(float)(gfx.ScreenWidth / 2), -(worldObjects.loc[i].y - cameraLoc.y) / (cameraZoomOut)+(float)(gfx.ScreenHeight / 2)), worldObjects.radius[i] * 2.0f / (cameraZoomOut), worldObjects.color[i], 0.8f); //(cameraZoomOut) / 500.0f
	}

	end = std::chrono::system_clock::now();
	std::chrono::duration<float> elapsedTime = end - start;
	dt = elapsedTime.count();

	start = std::chrono::system_clock::now(); // 500ms


	// Display number of active objects and MSEC PER FRAME, with other info. as needed

	RetroContent::DrawString(gfx, "NUMBER OF OBJECTS: " + std::to_string(worldObjects.currentAssignedObjects) , Vec2(200.0f, 20.0f), 2, Colors::Red);
	//RetroContent::DrawString(gfx, "PERCENT OPTIMISED: " + std::to_string(100 - (int)(((float)numCalcs * 100) / ((float)currentAssignedObjects * (float)(currentAssignedObjects - 1)))), Vec2(200.0f, 60.0f), 2, Colors::Red); // (int)(numCalcs * 100) / (currentAssignedObjects * (currentAssignedObjects - 1))
	RetroContent::DrawString(gfx, "TOTAL MSEC PER FRAME: " + std::to_string(int(dt * 1000.0f)), Vec2(800.0f, 20.0f), 2, Colors::Yellow);
	RetroContent::DrawString(gfx, "CHECK MSEC PER FRAME: " + std::to_string(int(optimiseDt * 1000.0f)), Vec2(800.0f, 80.0f), 2, Colors::Yellow);
}

