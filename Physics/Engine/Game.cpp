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

 /******************************************************************************************
 *	Game code and amendments by s0lly													   *
 *	https://www.youtube.com/channel/UCOyCXEB8NTx3Xjm8UptwsIg							   *
 *	https://s0lly.itch.io/																   *
 *	https://www.instagram.com/s0lly.gaming/												   *
 ******************************************************************************************/

#include "MainWindow.h"
#include "Game.h"

Game::Game(MainWindow& wnd)
	:
	wnd(wnd),
	gfx(wnd)
{
	// Initialise objects


	worldObjects.loc = new Vec2[numObjects];
	worldObjects.velocity = new Vec2[numObjects];
	worldObjects.mass = new float[numObjects];
	worldObjects.radius = new float[numObjects];
	worldObjects.color = new Color[numObjects];
	//renderComponents = new RenderComponent[numObjects];


	for (int i = 0; i < numObjects; i++)
	{
		int sizeOfField = 10000;

		float xRand = (float)(std::rand() % sizeOfField) - (float)(sizeOfField / 2);
		float yRand = (float)(std::rand() % sizeOfField) - (float)(sizeOfField / 2);
		
		float massRand = ((float)(std::rand() % 10000) + 1.0f);
		float radiusRand = std::sqrt(massRand / (PI / 1.0f));

		unsigned char rRand = (unsigned char)(std::rand() % 256);
		unsigned char gRand = (unsigned char)(std::rand() % 256);
		unsigned char bRand = (unsigned char)(std::rand() % 256);

		float magnitudeX = ((float)(std::rand() % 1001)) * 10.0f - 5000.0f;
		float magnitudeY = ((float)(std::rand() % 1001)) * 10.0f - 5000.0f;

		Vec2 startForce(magnitudeX, magnitudeY);

		startForce = Vec2();

		worldObjects.loc[currentAssignedObjects] = Vec2(xRand, yRand);
		worldObjects.velocity[currentAssignedObjects] = startForce;
		worldObjects.mass[currentAssignedObjects] = massRand;
		worldObjects.radius[currentAssignedObjects] = radiusRand;
		worldObjects.color[currentAssignedObjects] = Color(rRand, gRand, bRand);



		currentAssignedObjects++;
		//renderComponents[currentAssignedObjects] = { radiusRand,  Color(rRand, gRand, bRand) };
		
	}

	//float middleMass = 1000.0f;
	//worldObjects.push_back(WorldObject(Vec2(0.0f, 0.0f), std::sqrt(middleMass / PI), middleMass, Colors::Yellow));
	//worldObjects[worldObjects.size() - 1].ApplyForce(Force(0.0f, ClampAngle(0.0f)));
}

void Game::Go()
{
	gfx.BeginFrame();
	start = std::chrono::system_clock::now(); // 35ms
	ProcessInput();
	UpdateModel();
	ComposeFrame();
	end = std::chrono::system_clock::now();
	std::chrono::duration<float> elapsedTime = end - start;
	dt = elapsedTime.count(); //dt = 1.0f;//

	


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
	int numThreads = 50;
	int threadSize = currentAssignedObjects / numThreads + 1;
	auto worldObjectsPtr = &worldObjects;
	auto currentAssignedObjectsPtr = &currentAssignedObjects;
	auto dtPtr = &dt;

	std::vector<std::thread> threadList;
	for (int k = 0; k < numThreads; k++)
	{
		threadList.push_back(std::thread([worldObjectsPtr, k, currentAssignedObjectsPtr, dtPtr, threadSize]()
		{
			// Apply gravity forces to each object from all objects except from the object itself
			for (int i = k * threadSize; (i < (k + 1) * threadSize) && (i < (*currentAssignedObjectsPtr)); i++)
			{
				for (int j = 0; j < (*currentAssignedObjectsPtr); j++)
				{
					if (i != j)
					{
						ApplyGravityToFirst(worldObjectsPtr->loc[i], worldObjectsPtr->mass[i],
							worldObjectsPtr->velocity[i], worldObjectsPtr->loc[j], worldObjectsPtr->mass[j], (*dtPtr));
					}
				}
			}
		}));
	}
	std::for_each(threadList.begin(), threadList.end(), std::mem_fn(&std::thread::join));

	
	// Update all objects locations based on all active forces
	for (int i = 0; i < currentAssignedObjects; i++)
	{
		worldObjectsPtr->loc[i] = worldObjectsPtr->loc[i] + worldObjectsPtr->velocity[i] * dt;
	}

	// Merge objects that have collided
	//if (currentAssignedObjects > 1)
	//{
	//	for (int i = 0; i < currentAssignedObjects - 1; i++)
	//	{
	//		for (int j = i + 1; j < currentAssignedObjects; j++)
	//		{
	//			if (CheckCollision(worldObjects[i], worldObjects[j]))
	//			{
	//				WorldObject worldObjectMerged= MergeObjectsAndReturnNew(worldObjects[i], worldObjects[j]);
	//
	//				std::swap(worldObjects[j], worldObjects[currentAssignedObjects - 1]);
	//				currentAssignedObjects--;
	//				j--;
	//
	//				std::swap(worldObjects[i], worldObjects[currentAssignedObjects - 1]);
	//				currentAssignedObjects--;
	//				i--;
	//				j--;
	//
	//				worldObjects[currentAssignedObjects] = worldObjectMerged;
	//
	//				currentAssignedObjects++;
	//
	//				break;
	//			}
	//		}
	//	}
	//}
	
	// Lock camera on to largest object
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
	
	for (int i = 0; i < currentAssignedObjects; i++)
	{
		alignedMassXLoc += worldObjectsPtr->loc[i].x * worldObjectsPtr->mass[i];
		alignedMassYLoc += worldObjectsPtr->loc[i].y * worldObjectsPtr->mass[i];
		totalMass += worldObjectsPtr->mass[i];
	}
	alignedMassXLoc /= totalMass;
	alignedMassYLoc /= totalMass;
	
	Vec2 newLoc = Vec2(alignedMassXLoc, alignedMassYLoc); 

	// Smooth camera movement
	float percentageToMove = 0.1f;
	cameraLoc = Vec2((newLoc.x * percentageToMove + cameraLoc.x * (1.0f - percentageToMove)), (newLoc.y * percentageToMove + cameraLoc.y * (1.0f - percentageToMove)));

}

void Game::ComposeFrame()
{
	// Draw all objects
	for (int i = 0; i < currentAssignedObjects; i++)
	{
		gfx.DrawCircle(Vec2((worldObjects.loc[i].x - cameraLoc.x) / (cameraZoomOut) + (float)(gfx.ScreenWidth / 2), -(worldObjects.loc[i].y - cameraLoc.y) / (cameraZoomOut) + (float)(gfx.ScreenHeight / 2)), worldObjects.radius[i] / (cameraZoomOut), worldObjects.color[i]);
	}

	// Display number of active objects and MSEC PER FRAME
	RetroContent::DrawString(gfx, "NUM OBJ: " + std::to_string(currentAssignedObjects) , Vec2(150.0f, 20.0f), 3, Colors::Red);
	RetroContent::DrawString(gfx, "MSEC PER FRAME: " + std::to_string(int(dt * 1000.0f)), Vec2(800.0f, 20.0f), 3, Colors::Yellow);
}
