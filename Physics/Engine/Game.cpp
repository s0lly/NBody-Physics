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

	int seed = 300;
	srand(seed);

	for (int i = 0; i < numObjectsToAdd; i++)
	{
		int sizeOfField = 6000;

		float xRand = (float)(std::rand() % sizeOfField) - (float)(sizeOfField / 2);
		float yRand = (float)(std::rand() % sizeOfField) - (float)(sizeOfField / 2);
		
		float massRand = ((float)(std::rand() % 1000) + 1.0f) / 6.0f;
		float radiusRand = std::sqrt(massRand / PI);

		unsigned char rRand = (unsigned char)(std::rand() % 256);
		unsigned char gRand = (unsigned char)(std::rand() % 256);
		unsigned char bRand = (unsigned char)(std::rand() % 256);

		float magnitude = ((float)(std::rand() % 100)) * 5.0f;
		float angle = 0.0f;

		if (xRand > 0 && yRand > 0) // top right
		{
			angle = 5 * PI / 4;
		}
		if (xRand < 0 && yRand > 0) // top left
		{
			angle = 7 * PI / 4;
		}
		if (xRand > 0 && yRand < 0) // bot right
		{
			angle = 3 * PI / 4;
		}
		if (xRand < 0 && yRand < 0) // bot left
		{
			angle = 1 * PI / 4;
		}

		angle += ((float)(std::rand() % 100)) / 100.0f * PI / 2.0f - PI / 4.0f;

		worldObjects.push_back(WorldObject(Vec2(xRand, yRand), radiusRand, massRand, Color(rRand, gRand, bRand)));
		worldObjects[worldObjects.size() - 1].ApplyForce(Force(magnitude / massRand, ClampAngle(angle)));
		
	}

	float middleMass = 1000.0f;
	worldObjects.push_back(WorldObject(Vec2(0.0f, 0.0f), std::sqrt(middleMass / PI), middleMass, Colors::Yellow));
	worldObjects[worldObjects.size() - 1].ApplyForce(Force(0.0f, ClampAngle(0.0f)));
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
	// Create thread information for running gravity calculations
	int numThreads = 20;
	int size = (int)worldObjects.size();
	int threadSize = size / numThreads + 1;

	auto worldObjectsPtr = &worldObjects;

	std::vector<std::thread> threadList;
	for (int k = 0; k < numThreads; k++)
	{
		threadList.push_back(std::thread([worldObjectsPtr, k, threadSize]()
		{
			// Apply gravity forces to each object from all objects except from the object itself
			for (int i = k * threadSize; (i < (k + 1) * threadSize) && (i < worldObjectsPtr->size()); i++)
			{
				for (int j = 0; j < worldObjectsPtr->size(); j++)
				{
					if (i != j)
					{
						ApplyGravity((*worldObjectsPtr)[i], (*worldObjectsPtr)[j]);
					}
				}
			}
		}));
	}
	std::for_each(threadList.begin(), threadList.end(), std::mem_fn(&std::thread::join));
	
	// Update all objects locations based on all active forces
	for (auto& worldObject : worldObjects)
	{
		worldObject.Update();
	}

	// Merge objects that have collided
	if (worldObjects.size() > 1)
	{
		for (int i = 0; i < worldObjects.size() - 1; i++)
		{
			for (int j = i + 1; j < worldObjects.size(); j++)
			{
				if (CheckCollision(worldObjects[i], worldObjects[j]))
				{
					worldObjects.push_back(MergeObjectsAndReturnNew(worldObjects[i], worldObjects[j]));

					std::iter_swap(worldObjects.begin() + j, worldObjects.end() - 1);
					worldObjects.pop_back();
					j--;

					std::iter_swap(worldObjects.begin() + i, worldObjects.end() - 1);
					worldObjects.pop_back();
					i--;
					j--;

					break;
				}
			}
		}
	}
	
	// Lock camera on to largest object
	float alignedMass = 0.0f;
	Vec2 newLoc = cameraLoc;
	for (auto& worldObject : worldObjects)
	{
		if (alignedMass < worldObject.mass) 
		{
			alignedMass = worldObject.mass;
			newLoc = worldObject.loc;
		}
	}

	// Smooth camera movement
	float percentageToMove = 0.1f;
	cameraLoc = Vec2((newLoc.x * percentageToMove + cameraLoc.x * (1.0f - percentageToMove)), (newLoc.y * percentageToMove + cameraLoc.y * (1.0f - percentageToMove)));

}

void Game::ComposeFrame()
{
	// Draw all objects
	for (auto& worldObject : worldObjects)
	{
		gfx.DrawCircle(Vec2((worldObject.loc.x - cameraLoc.x) / (cameraZoomOut) + (float)(gfx.ScreenWidth / 2), -(worldObject.loc.y - cameraLoc.y) / (cameraZoomOut) + (float)(gfx.ScreenHeight / 2)), worldObject.radius / (cameraZoomOut), worldObject.color);
	}

	// Display number of active objects
	RetroContent::DrawString(gfx, std::to_string(worldObjects.size()) , Vec2(100.0f, 20.0f), 3, Colors::Red);
}
