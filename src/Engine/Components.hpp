#pragma once

#include "MyMath/MyVectors.hpp"
#include "raylib.h"

#include "Types.hpp"

#include <vector>

namespace Component
{
	struct Transform
	{
		Vec2<float> position;
		Vec2<float> velocity;
		float rotation = 0;

		constexpr bool operator<=>(const Transform&) const = default;
	};

	struct Sprite
	{
		Texture2D texture = {};
		Rectangle rectangle = {0, 0, 0, 0};
		Color color = WHITE;
		float scale = 1;
		u32 layer = 1;
	};

	struct Animation
	{
		Texture2D texture = {};
		std::vector<Rectangle> frames;
		float frameDuration = 1;
		bool loop = false;

		float time = 0;
		bool playing = true;
		float speed = 1;
	};
}