/****************************************************************************************** 
 *	Chili DirectX Framework Version 16.07.20											  *	
 *	Game.h																				  *
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

#pragma once

#include "Keyboard.h"
#include "Mouse.h"
#include "Graphics.h"
#include "WorldObject.h"
#include "RetroContent.h"
#include "Tree.h"
#include <string>
#include <thread>
#include <vector>

class Game
{
public:
	Game( class MainWindow& wnd );
	Game( const Game& ) = delete;
	Game& operator=( const Game& ) = delete;
	void Go();
private:
	void ComposeFrame();
	void UpdateModel();
	void ProcessInput();
	/********************************/
	/*  User Functions              */
	/********************************/
private:
	MainWindow& wnd;
	Graphics gfx;
	/********************************/
	/*  User Variables              */
	/********************************/

	std::chrono::system_clock::time_point start;
	std::chrono::system_clock::time_point end;
	float dt = 1.0f;

	std::chrono::system_clock::time_point optimiseStart;
	std::chrono::system_clock::time_point optimiseEnd;
	float optimiseDt = 1.0f;

	// Debug time info using OOP design:
	// 3000 objects = 140ms
	// 1500 objects = 33.3ms
	// Just under 1000 objects = 16.66ms


	// Redesign away from OOP and use vector math, excl. mt:
	// 1500 objects = 20.0ms


	// Re-incl. mt:
	// 3000 objects = 18.0ms


	Vec2 cameraLoc = Vec2(0.0f, 0.0f);
	float cameraZoomOut = 480.0f;

	WorldObjects worldObjects;

	int numObjects = 250000;

	Tree tree;



};