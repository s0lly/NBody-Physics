
/*******************************************************************************************
*	Code and amendments by s0lly														   *
*	https://www.youtube.com/c/s0lly							                               *
*	https://s0lly.itch.io/																   *
*	https://www.instagram.com/s0lly.gaming/												   *
********************************************************************************************/

#pragma once

#include "Vec2.h"


struct WorldObjects
{
	Vec2 *loc;
	Vec2 *oldVelocity;
	Vec2 *velocity;
	float *mass;
	float *radius;
	Color *color;
	int *nodeID;
	int *calcsCompleted;
	int currentAssignedObjects;

	void Init(int numObjects)
	{
		loc = new Vec2[numObjects]();
		oldVelocity = new Vec2[numObjects]();
		velocity = new Vec2[numObjects]();
		mass = new float[numObjects]();
		radius = new float[numObjects]();
		color = new Color[numObjects]();
		nodeID = new int[numObjects]();
		calcsCompleted = new int[numObjects]();
		currentAssignedObjects = 0;

		for (int i = 0; i < numObjects; i++)
		{
			int sizeOfField = 10000;

			float xRand = ((float)(std::rand() % sizeOfField) - (float)(sizeOfField / 2));
			float yRand = ((float)(std::rand() % sizeOfField) - (float)(sizeOfField / 2));

			Vec2 locRand(xRand, yRand);
			locRand = locRand / sqrt(locRand.x * locRand.x + locRand.y * locRand.y);
			locRand.x = locRand.x * (float)(std::rand() % sizeOfField) * 10.0f;
			locRand.y = locRand.y * (float)(std::rand() % sizeOfField) * 1.0f;

			float massRand = ((float)(std::rand() % 10000) + 1.0f);
			float radiusRand = std::sqrt(massRand / (PI / 20.0f));

			unsigned char bRand = 64 + (unsigned char)(std::rand() % 192);
			unsigned char gRand = 128 + (unsigned char)(std::rand() % 128);
			unsigned char rRand = 192 + (unsigned char)(std::rand() % 64);

			xRand = abs(xRand) < 1.0f ? 1.0f : xRand;
			yRand = abs(yRand) < 1.0f ? 1.0f : yRand;

			//locRand.x = locRand.x + (((float)(std::rand() % 5) - 2.0f) * ((float)sizeOfField / 2.0f)) * 8.0f;
			//locRand.y = locRand.y + (((float)(std::rand() % 2) - 1.0f) * ((float)sizeOfField / 2.0f)) * 8.0f;

			float magnitudeX = -(locRand.y) * 0.03f;
			float magnitudeY = +(locRand.x) * 0.03f;

			Vec2 startForce(magnitudeX, magnitudeY);

			//startForce = Vec2();




			loc[currentAssignedObjects] = locRand;
			velocity[currentAssignedObjects] = startForce;
			mass[currentAssignedObjects] = massRand;
			radius[currentAssignedObjects] = radiusRand;
			color[currentAssignedObjects] = Color(rRand, gRand, bRand);

			currentAssignedObjects++;
		}
	}
};



static float
ClampAngle(float angle)
{
	angle = angle < 0.0f ? (angle + PI * 2.0f) : angle;
	angle = angle >= PI * 2.0f ? (angle - PI * 2.0f) : angle;

	return angle;
}


static void
ApplyForce(Vec2 &velocity, float &mass, Vec2 in_force, float dt)
{
	velocity = velocity + (in_force * dt) / mass;
}


static bool
ApplyGravityToFirst(Vec2 *locFirst, float *massFirst, Vec2 *velocityFirst, int *calcsCompletedFirst,
					Vec2 locSecond, float massSecond, float dt, bool isNode)
{
	// Ensures that infinity speed is not possible
	float minRadiusSqrdAllowed = 0.01f;

	Vec2 vectorFromFirstToSecond = locSecond - *locFirst;

	float radiusSqrd = GetVec2Magnitude(vectorFromFirstToSecond);
	radiusSqrd = radiusSqrd < minRadiusSqrdAllowed ? minRadiusSqrdAllowed : radiusSqrd;

	float gravityMagnitude = 1.0f * (GRAV_CONST * (*massFirst) * massSecond) / radiusSqrd; // The constant can be changed to effect all objects masses

	Vec2 gravOnFirstToSecond = GetNormalizedVec2(vectorFromFirstToSecond) * gravityMagnitude;

	if (isNode)
	{
		if (gravityMagnitude < NODE_GRAV_THRESHOLD)
		{
			ApplyForce((*velocityFirst), (*massFirst), gravOnFirstToSecond, dt);
			(*calcsCompletedFirst)++;
			return true;
		}
		else
		{
			return false;
		}
		
	}
	else
	{
		ApplyForce((*velocityFirst), (*massFirst), gravOnFirstToSecond, dt);
		(*calcsCompletedFirst)++;
		return true;
	}
	
}


//static bool
//CheckCollision(WorldObject& first, WorldObject& second)
//{
//	float radiusSqrd = (first.loc.x - second.loc.x) * (first.loc.x - second.loc.x) + (first.loc.y - second.loc.y) * (first.loc.y - second.loc.y);
//	float radiusCollideSqrd = (first.radius + second.radius) * (first.radius + second.radius);
//	
//	return (radiusSqrd <= radiusCollideSqrd);
//}
//
//
//static WorldObject
//MergeObjectsAndReturnNew(WorldObject& first, WorldObject& second)
//{
//
//	Vec2 newLoc = Vec2((first.loc.x * first.mass + second.loc.x * second.mass) / (first.mass + second.mass), (first.loc.y * first.mass + second.loc.y * second.mass) / (first.mass + second.mass));
//	float newMass = first.mass + second.mass;
//
//	float newAreaSqrd = first.radius * first.radius + second.radius * second.radius;
//	float newRadius = std::sqrt(newAreaSqrd);
//	//float newRadius = std::sqrt(newMass / PI);
//
//	unsigned char newR = (unsigned char)(((float)first.color.GetR() * (first.mass) + (float)second.color.GetR() * second.mass) / (first.mass + second.mass));
//	unsigned char newG = (unsigned char)(((float)first.color.GetG() * (first.mass) + (float)second.color.GetG() * second.mass) / (first.mass + second.mass));
//	unsigned char newB = (unsigned char)(((float)first.color.GetB() * (first.mass) + (float)second.color.GetB() * second.mass) / (first.mass + second.mass));
//	
//	Color newColor = Color(newR, newG, newB);
//
//	Vec2 newVelocity((first.velocity * first.mass + second.velocity * second.mass) / newMass);
//
//	return WorldObject{ newLoc, newVelocity, newMass, newRadius, newColor };
//}