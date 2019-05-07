
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
	float minRadiusSqrdAllowed = 0.1f;

	Vec2 vectorFromFirstToSecond = locSecond - *locFirst;

	float radiusSqrd = GetVec2Magnitude(vectorFromFirstToSecond);
	radiusSqrd = radiusSqrd < minRadiusSqrdAllowed ? minRadiusSqrdAllowed : radiusSqrd;

	float gravityMagnitude = 10.0f * (GRAV_CONST * (*massFirst) * massSecond) / radiusSqrd; // The constant can be changed to effect all objects masses

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