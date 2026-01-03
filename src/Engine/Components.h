#pragma once

#include "Raylib/raylib.h"
#include "MyMath/MyVectors.h"

#include "Types.h"

namespace Component
{
	struct Transform
	{
		Vector2f position;
		Vector2f velocity;
		float rotation = 0;
	};

	struct Sprite
	{
		Texture2D texture;
		Rectangle rectangle = {0, 0, 0, 0};
		Color color = WHITE;
		float scale = 1;
		u32 layer = 1;
	};

	struct Animation
	{
		bool active = false;
		u32 startingIndex = 0;
		u32 endingIndex = 1;
		u32 currentIndex = 0;
		float frameLengthS = 1;
		float frameAccumulator = 0;
	};
}