
/*******************************************************************************************
*	Code and amendments by s0lly														   *
*	https://www.youtube.com/c/s0lly							                               *
*	https://s0lly.itch.io/																   *
*	https://www.instagram.com/s0lly.gaming/												   *
********************************************************************************************/

#pragma once

#include <math.h>

#define MAGNITUDE_THRESHOLD 0.00001f

#define PI 3.1415926f

#define GRAV_CONST 0.004302f

#define NODE_GRAV_THRESHOLD 1000000.0f


static float numCalcs = 0.0f;




struct Vec2
{
	Vec2()
	{
		x = 0.0f;
		y = 0.0f;
	}
	Vec2(float a, float b)
		:
		x(a),
		y(b)
	{
	}

	Vec2 operator +(Vec2& rhs)
	{
		return Vec2(x + rhs.x, y + rhs.y);
	}

	Vec2 operator -(Vec2& rhs)
	{
		return Vec2(x - rhs.x, y - rhs.y);
	}

	Vec2 operator *(float rhs)
	{
		return Vec2(x * rhs, y * rhs);
	}

	Vec2 operator /(float rhs)
	{
		return Vec2(x / rhs, y / rhs);
	}


	float x;
	float y;
};


static float
GetVec2MagnitudeSqrd(const Vec2 &vec)
{
	return vec.x * vec.x + vec.y * vec.y;
}


static float
GetVec2Magnitude(const Vec2 &vec)
{
	return sqrt(GetVec2MagnitudeSqrd(vec));
}


static Vec2
GetNormalizedVec2(const Vec2 &vec)
{
	float magnitude = GetVec2Magnitude(vec);
	if (magnitude > MAGNITUDE_THRESHOLD)
	{
		return Vec2(vec.x / magnitude, vec.y / magnitude);
	}
	else
	{
		return Vec2(0.0f, 0.0f);
	}
}


static float
GetDotVec2(Vec2& lhs, Vec2& rhs)
{
	Vec2 norm = GetNormalizedVec2(lhs);
	Vec2 otherNorm = GetNormalizedVec2(rhs);

	return norm.x * otherNorm.x + norm.y * otherNorm.y;
}