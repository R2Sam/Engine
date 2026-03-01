#pragma once

#include "MyMath/MyVectors.h"
#include "raylib.h"

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
		Texture2D texture = {};
		Rectangle rectangle = {0, 0, 0, 0};
		Color color = WHITE;
		float scale = 1;
		u32 layer = 1;
		// When editing texture, layer or any value
		// registry.replace<Component::Sprite>(entity, newSprite);
		// registry.patch<Component::Sprite>(entity, [](auto& sprite){sprite.layer = 5});
		// with the updated reference for the sprites to be reordered correctly.
		// Do not create a multi owning entt group (registry.group<Component::Sprite, ...>(....))
		// as this will reorder the sprites.
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