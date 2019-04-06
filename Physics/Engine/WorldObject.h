
/******************************************************************************************
*	Game code and amendments by s0lly													   *
*	https://www.youtube.com/channel/UCOyCXEB8NTx3Xjm8UptwsIg							   *
*	https://s0lly.itch.io/																   *
*	https://www.instagram.com/s0lly.gaming/												   *
******************************************************************************************/

#pragma once

#include "Vec2.h"
#include "Force.h"

static const float gravConstant = 0.004302f; // As per wikipedia's "Gravitational Constant" for solar masses

class WorldObject
{
public:
	WorldObject(Vec2 in_loc, float in_radius, float in_mass, Color in_color, ForceComponent horizontal = ForceComponent(0, 1, FORCECOMPONENTTYPE::HORIZONTAL), ForceComponent vertical = ForceComponent(0, 1, FORCECOMPONENTTYPE::VERTICAL))
		:
		loc(in_loc),
		radius(in_radius),
		mass(in_mass),
		color(in_color),
		forceHorizontalComponent(horizontal),
		forceVerticalComponent(vertical)
	{
	}

	void Update()
	{
		loc.x += forceHorizontalComponent.magnitude * forceHorizontalComponent.direction;
		loc.y += forceVerticalComponent.magnitude * forceVerticalComponent.direction;
	}

	void ApplyForce(Force in_force)
	{
		forceHorizontalComponent = forceHorizontalComponent + in_force.GetHorizonalComponent();
		forceVerticalComponent = forceVerticalComponent + in_force.GetVerticalComponent();
	}

	Vec2 loc;
	float radius;
	float mass;
	Color color;
	ForceComponent forceHorizontalComponent;
	ForceComponent forceVerticalComponent;

private:

};

static float ClampAngle(float angle)
{
	angle = angle < 0.0f ? angle + PI * 2.0f : angle;
	angle = angle >= PI * 2.0f ? angle - PI * 2.0f : angle;

	return angle;
}

// Only applies gravity effects to the first object (can easily be extended to apply to both by uncommenting the last statement) 
static void ApplyGravity(WorldObject& first, WorldObject& second)
{
	// Ensures that infinity speed is not possible
	float minRadiusAllowed = 0.00001f;

	float radiusSqrd = (first.loc.x - second.loc.x) * (first.loc.x - second.loc.x) + (first.loc.y - second.loc.y) * (first.loc.y - second.loc.y);
	radiusSqrd = radiusSqrd < minRadiusAllowed ? minRadiusAllowed : radiusSqrd;

	float radius = std::sqrt(radiusSqrd);

	float gravityMagnitude = 100.0f * (gravConstant * first.mass * second.mass) / radiusSqrd; // The constant can be changed to effect all objects masses

	float angleFromAcos = acosf(-(second.loc.y - first.loc.y) / radius);
	int xDir = (second.loc.x - first.loc.x) < 0 ? -1 : 1;

	float directionOfSecondFromFirst = ClampAngle(angleFromAcos * (float)xDir);

	// Divide acceleration by the object's mass
	first.ApplyForce(Force(gravityMagnitude / first.mass, directionOfSecondFromFirst));
	//second.ApplyForce(Force(gravityMagnitude / second.mass, ClampAngle(directionOfSecondFromFirst - PI)));
}

static bool CheckCollision(WorldObject& first, WorldObject& second)
{
	float radiusSqrd = (first.loc.x - second.loc.x) * (first.loc.x - second.loc.x) + (first.loc.y - second.loc.y) * (first.loc.y - second.loc.y);
	float radiusCollideSqrd = (first.radius + second.radius) * (first.radius + second.radius);
	
	return (radiusSqrd <= radiusCollideSqrd);
}

static WorldObject MergeObjectsAndReturnNew(WorldObject& first, WorldObject& second)
{
	// All new objects are combined by weighting by each component objects' masses
	Vec2 newLoc = Vec2((first.loc.x * first.mass + second.loc.x * second.mass) / (first.mass + second.mass), (first.loc.y * first.mass + second.loc.y * second.mass) / (first.mass + second.mass));
	float newMass = first.mass + second.mass;

	float newAreaExPI = first.radius * first.radius + second.radius * second.radius;
	float newRadius = std::sqrt(newAreaExPI);

	unsigned char newR = (unsigned char)(((float)first.color.GetR() * (first.mass) + (float)second.color.GetR() * second.mass) / (first.mass + second.mass));
	unsigned char newG = (unsigned char)(((float)first.color.GetG() * (first.mass) + (float)second.color.GetG() * second.mass) / (first.mass + second.mass));
	unsigned char newB = (unsigned char)(((float)first.color.GetB() * (first.mass) + (float)second.color.GetB() * second.mass) / (first.mass + second.mass));
	
	Color newColor = Color(newR, newG, newB);
	
	float forceHorizontalComponentNewMagnitude = (first.forceHorizontalComponent.magnitude * first.mass * (float)(first.forceHorizontalComponent.direction)
		+ second.forceHorizontalComponent.magnitude * second.mass * (float)(second.forceHorizontalComponent.direction)) / (first.mass + second.mass);
	int forceHorizontalComponentNewDirection;
	if (forceHorizontalComponentNewMagnitude < 0)
	{
		forceHorizontalComponentNewMagnitude = forceHorizontalComponentNewMagnitude * (-1.0f);
		forceHorizontalComponentNewDirection = -1;
	}
	else
	{
		forceHorizontalComponentNewDirection = 1;
	}

	float forceVerticalComponentNewMagnitude = (first.forceVerticalComponent.magnitude * first.mass * (float)(first.forceVerticalComponent.direction)
		+ second.forceVerticalComponent.magnitude * second.mass * (float)(second.forceVerticalComponent.direction)) / (first.mass + second.mass);
	int forceVerticalComponentNewDirection;
	if (forceVerticalComponentNewMagnitude < 0)
	{
		forceVerticalComponentNewMagnitude = forceVerticalComponentNewMagnitude * (-1.0f);
		forceVerticalComponentNewDirection = -1;
	}
	else
	{
		forceVerticalComponentNewDirection = 1;
	}


	ForceComponent forceHorizontalComponentNew = ForceComponent(forceHorizontalComponentNewMagnitude, forceHorizontalComponentNewDirection, first.forceHorizontalComponent.type);
	ForceComponent forceVerticalComponentNew = ForceComponent(forceVerticalComponentNewMagnitude, forceVerticalComponentNewDirection, first.forceVerticalComponent.type);

	return WorldObject(newLoc, newRadius, newMass, newColor, forceHorizontalComponentNew, forceVerticalComponentNew);
}