#pragma once

#include "MyMath/MyVectors.hpp"
#include "raylib.h"

#include "Types.hpp"

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

		// Don't sort anything that has an owning sprite
		// as this would mess up the sorting of layers

		constexpr bool operator<=>(const Sprite&) const = delete;
	};

	struct Animation
	{
		bool active = false;
		bool loop = false;
		bool restart = false;
		u32 startingIndex = 0;
		u32 endingIndex = 1;
		u32 currentIndex = startingIndex;
		float frameLengthS = 1;
		float frameAccumulator = 0;

		constexpr bool operator<=>(const Animation&) const = default;
	};
}