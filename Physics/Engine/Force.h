
/******************************************************************************************
*	Game code and amendments by s0lly													   *
*	https://www.youtube.com/channel/UCOyCXEB8NTx3Xjm8UptwsIg							   *
*	https://s0lly.itch.io/																   *
*	https://www.instagram.com/s0lly.gaming/												   *
******************************************************************************************/

#pragma once

static const float PI = 3.1415926f;

enum FORCECOMPONENTTYPE
{
	HORIZONTAL,
	VERTICAL
};

struct ForceComponent
{
	ForceComponent(float in_magnitude, int in_direction, FORCECOMPONENTTYPE in_type)
		:
		magnitude(in_magnitude),
		direction(in_direction),
		type(in_type)
	{
	}

	ForceComponent operator+(const ForceComponent& other)
	{
		if (type == other.type)
		{
			float newMagnitude = magnitude + other.magnitude * (float)(other.direction * direction);
			int newDirection = direction;

			if (magnitude < 0)
			{
				newMagnitude = newMagnitude * (-1.0f);
				newDirection = newDirection * (-1);
			}

			return ForceComponent(newMagnitude, newDirection, type);
		}
		else
		{
			return ForceComponent(0, 1, FORCECOMPONENTTYPE::VERTICAL);
		}
	}

	float magnitude;
	int direction;
	FORCECOMPONENTTYPE type = FORCECOMPONENTTYPE::VERTICAL;
};

struct Force
{
	Force(float in_magnitude, float in_direction)
		:
		magnitude(in_magnitude),
		direction(in_direction)
	{
	}

	ForceComponent GetHorizonalComponent()
	{
		float horizonalMagnitude = magnitude * abs(sin(direction));
		int horizonalDirection = sinf(direction) > 0.0f ? 1 : -1;

		return ForceComponent(horizonalMagnitude, horizonalDirection, FORCECOMPONENTTYPE::HORIZONTAL);
	}

	ForceComponent GetVerticalComponent()
	{
		float verticalMagnitude = magnitude * abs(cos(direction));
		int verticalDirection = cosf(direction) > 0.0f ? -1 : 1;

		return ForceComponent(verticalMagnitude, verticalDirection, FORCECOMPONENTTYPE::VERTICAL);
	}

	float magnitude;
	float direction;

};

