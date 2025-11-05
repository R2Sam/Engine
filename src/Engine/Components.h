#pragma once

#include "Raylib/raylib.h"

#include "MyMath/MyVectors.h"

namespace Component
{
	struct Transform
	{
		Vector2f position;
		float rotation = 0;
	};

	struct Sprite
	{
		Texture2D texture;
		Rectangle rectangle;
		Color color = WHITE;
		float scale = 1;
	};
}