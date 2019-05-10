
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
		currentAssignedObjects = 0;

		for (int i = 0; i < numObjects; i++)
		{
			int sizeOfField = 10000;

			float xRand = ((float)(std::rand() % sizeOfField) - (float)(sizeOfField / 2));
			float yRand = ((float)(std::rand() % sizeOfField) - (float)(sizeOfField / 2));

			Vec2 locRand(xRand, yRand);
			locRand = locRand / sqrt(locRand.x * locRand.x + locRand.y * locRand.y);
			locRand.x = locRand.x * (float)(std::rand() % sizeOfField) * 20.0f;
			locRand.y = locRand.y * (float)(std::rand() % sizeOfField) * 4.0f;

			float massRand = ((float)(std::rand() % 10000) + 1.0f);
			float radiusRand = std::sqrt(massRand / (PI / 2.0f));

			unsigned char bRand = 64 + (unsigned char)(std::rand() % 192);
			unsigned char gRand = 128 + (unsigned char)(std::rand() % 128);
			unsigned char rRand = 192 + (unsigned char)(std::rand() % 64);

			xRand = abs(xRand) < 1.0f ? 1.0f : xRand;
			yRand = abs(yRand) < 1.0f ? 1.0f : yRand;

			//locRand.x = locRand.x + (((float)(std::rand() % 5) - 2.0f) * ((float)sizeOfField / 2.0f)) * 8.0f;
			//locRand.y = locRand.y + (((float)(std::rand() % 2) - 1.0f) * ((float)sizeOfField / 2.0f)) * 8.0f;

			float magnitudeX = -(locRand.y) * 0.01f;
			float magnitudeY = +(locRand.x) * 0.002f;

			Vec2 startForce(magnitudeX, magnitudeY);

			//startForce = Vec2();




			loc[currentAssignedObjects] = locRand;
			velocity[currentAssignedObjects] = startForce;
			mass[currentAssignedObjects] = massRand;
			radius[currentAssignedObjects] = radiusRand;
			color[currentAssignedObjects] = Colors::White;//Color(rRand, gRand, bRand);

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
ApplyGravityToFirst(Vec2 *locFirst, float *massFirst, Vec2 *velocityFirst,
					Vec2 locSecond, float massSecond, float dt, bool isNode)
{
	// Ensures that infinity speed is not possible
	float minRadiusSqrdAllowed = 0.001f;

	Vec2 vectorFromFirstToSecond = locSecond - *locFirst;

	float radiusSqrd = GetVec2Magnitude(vectorFromFirstToSecond);
	//radiusSqrd = radiusSqrd < minRadiusSqrdAllowed ? minRadiusSqrdAllowed : radiusSqrd;

	if (radiusSqrd > minRadiusSqrdAllowed)
	{
		float gravityMagnitude = 1.0f * (GRAV_CONST * (*massFirst) * massSecond) / radiusSqrd; // The constant can be changed to effect all objects masses

		Vec2 gravOnFirstToSecond = GetNormalizedVec2(vectorFromFirstToSecond) * gravityMagnitude;

		if (isNode)
		{
			if (gravityMagnitude < NODE_GRAV_THRESHOLD)
			{
				ApplyForce((*velocityFirst), (*massFirst), gravOnFirstToSecond, dt);
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
			return true;
		}
	}

	
}


static bool
CheckCollision(WorldObjects &worldObjects, int first, int second)
{
	float radiusSqrd = (worldObjects.loc[first].x - worldObjects.loc[second].x) * (worldObjects.loc[first].x - worldObjects.loc[second].x) + (worldObjects.loc[first].y - worldObjects.loc[second].y) * (worldObjects.loc[first].y - worldObjects.loc[second].y);
	float radiusCollideSqrd = (worldObjects.radius[first] + worldObjects.radius[second]) * (worldObjects.radius[first] + worldObjects.radius[second]);
	
	return (radiusSqrd <= radiusCollideSqrd);
}


static void
MergeObjects(WorldObjects *worldObjectsPtr, int first, int second)
{

	Vec2 newLoc = Vec2((worldObjectsPtr->loc[first].x * worldObjectsPtr->mass[first] + worldObjectsPtr->loc[second].x * worldObjectsPtr->mass[second]) / (worldObjectsPtr->mass[first] + worldObjectsPtr->mass[second]), (worldObjectsPtr->loc[first].y * worldObjectsPtr->mass[first] + worldObjectsPtr->loc[second].y * worldObjectsPtr->mass[second]) / (worldObjectsPtr->mass[first] + worldObjectsPtr->mass[second]));
	float newMass = worldObjectsPtr->mass[first] + worldObjectsPtr->mass[second];

	//float newAreaSqrd = worldObjectsPtr->radius[first] * worldObjectsPtr->radius[first] + worldObjectsPtr->radius[second] * worldObjectsPtr->radius[second];
	float newRadius = std::sqrt(newMass / (PI / 2.0f));
	//float newRadius = std::sqrt(newMass / PI);

	unsigned char newR = (unsigned char)(((float)worldObjectsPtr->color[first].GetR() * (worldObjectsPtr->mass[first]) + (float)worldObjectsPtr->color[second].GetR() * worldObjectsPtr->mass[second]) / (worldObjectsPtr->mass[first] + worldObjectsPtr->mass[second]));
	unsigned char newG = (unsigned char)(((float)worldObjectsPtr->color[first].GetG() * (worldObjectsPtr->mass[first]) + (float)worldObjectsPtr->color[second].GetG() * worldObjectsPtr->mass[second]) / (worldObjectsPtr->mass[first] + worldObjectsPtr->mass[second]));
	unsigned char newB = (unsigned char)(((float)worldObjectsPtr->color[first].GetB() * (worldObjectsPtr->mass[first]) + (float)worldObjectsPtr->color[second].GetB() * worldObjectsPtr->mass[second]) / (worldObjectsPtr->mass[first] + worldObjectsPtr->mass[second]));
	
	Color newColor = Color(newR, newG, newB);

	Vec2 newOldVelocity((worldObjectsPtr->oldVelocity[first] * worldObjectsPtr->mass[first] + worldObjectsPtr->oldVelocity[second] * worldObjectsPtr->mass[second]) / newMass);
	Vec2 newVelocity((worldObjectsPtr->velocity[first] * worldObjectsPtr->mass[first] + worldObjectsPtr->velocity[second] * worldObjectsPtr->mass[second]) / newMass);

	std::swap(worldObjectsPtr->loc[second], worldObjectsPtr->loc[worldObjectsPtr->currentAssignedObjects - 1]);
	std::swap(worldObjectsPtr->oldVelocity[second], worldObjectsPtr->oldVelocity[worldObjectsPtr->currentAssignedObjects - 1]);
	std::swap(worldObjectsPtr->velocity[second], worldObjectsPtr->velocity[worldObjectsPtr->currentAssignedObjects - 1]);
	std::swap(worldObjectsPtr->mass[second], worldObjectsPtr->mass[worldObjectsPtr->currentAssignedObjects - 1]);
	std::swap(worldObjectsPtr->radius[second], worldObjectsPtr->radius[worldObjectsPtr->currentAssignedObjects - 1]);
	std::swap(worldObjectsPtr->color[second], worldObjectsPtr->color[worldObjectsPtr->currentAssignedObjects - 1]);
	std::swap(worldObjectsPtr->nodeID[second], worldObjectsPtr->nodeID[worldObjectsPtr->currentAssignedObjects - 1]);

	worldObjectsPtr->currentAssignedObjects--;


	std::swap(worldObjectsPtr->loc[first], worldObjectsPtr->loc[worldObjectsPtr->currentAssignedObjects - 1]);
	std::swap(worldObjectsPtr->oldVelocity[first], worldObjectsPtr->oldVelocity[worldObjectsPtr->currentAssignedObjects - 1]);
	std::swap(worldObjectsPtr->velocity[first], worldObjectsPtr->velocity[worldObjectsPtr->currentAssignedObjects - 1]);
	std::swap(worldObjectsPtr->mass[first], worldObjectsPtr->mass[worldObjectsPtr->currentAssignedObjects - 1]);
	std::swap(worldObjectsPtr->radius[first], worldObjectsPtr->radius[worldObjectsPtr->currentAssignedObjects - 1]);
	std::swap(worldObjectsPtr->color[first], worldObjectsPtr->color[worldObjectsPtr->currentAssignedObjects - 1]);
	std::swap(worldObjectsPtr->nodeID[first], worldObjectsPtr->nodeID[worldObjectsPtr->currentAssignedObjects - 1]);

	worldObjectsPtr->currentAssignedObjects--;


	worldObjectsPtr->loc[worldObjectsPtr->currentAssignedObjects] = newLoc;
	worldObjectsPtr->oldVelocity[worldObjectsPtr->currentAssignedObjects] = newOldVelocity;
	worldObjectsPtr->velocity[worldObjectsPtr->currentAssignedObjects] = newVelocity;
	worldObjectsPtr->mass[worldObjectsPtr->currentAssignedObjects] = newMass;
	worldObjectsPtr->radius[worldObjectsPtr->currentAssignedObjects] = newRadius;
	worldObjectsPtr->color[worldObjectsPtr->currentAssignedObjects] = newColor;
	worldObjectsPtr->nodeID[worldObjectsPtr->currentAssignedObjects] = -1; // this should be irrelevant as is reset before used

	worldObjectsPtr->currentAssignedObjects++;
}