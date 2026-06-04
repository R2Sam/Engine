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

	struct ParticleEmitter
	{
		float radius = 1;
		bool edgeOnly = false;

		float spawnRate = 10;
		u32 burst = 0;
		bool playing = true;

		float lifetimeMin = 0.5;
		float lifetimeMax = 1.5;

		float speedMin = 50;
		float speedMax = 150;

		bool useDirection = false;
		float directionAngle = 0;
		float directionSpread = 45;

		Color startColor = WHITE;
		Color endColor = {255, 255, 255, 0};
		float startSize = 4;
		float endSize = 1;

		float gravity = 0;

		float spawnAccumulator = 0;
	};

	struct Particle
	{
		Vec2<float> position;
		Vec2<float> velocity;
		float lifetime = 1;
		float age = 0;

		Color startColor = WHITE;
		Color endColor = {255, 255, 255, 0};
		float startSize = 1;
		float endSize = 1;
	};

}