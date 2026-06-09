#pragma once

#include "MyMath/MyVectors.hpp"
#include "raylib.h"

#include "Types.hpp"

#include <Engine/Registry.hpp>
#include <Networking/Encryption.hpp>
#include <functional>
#include <vector>

/**
 * @file Components.hpp
 * @brief Engine components.
 */

namespace Component
{
	/**
	 * @struct Transform
	 * @brief Position, velocity and rotation.
	 */
	struct Transform
	{
		Vec2<float> position;
		Vec2<float> velocity;
		float rotation = 0;

		template <class Archive>
		void serialize(Archive& archive) // NOLINT
		{
			archive(position, velocity, rotation);
		}

		constexpr bool operator<=>(const Transform&) const = default;
	};

	/**
	 * @struct Sprite
	 * @brief Texture, source rectangle, tint, scale and render layer.
	 */
	struct Sprite
	{
		Texture2D texture = {};
		Rectangle rectangle = {0, 0, 0, 0};
		Color color = WHITE;
		float scale = 1;
		u32 layer = 1;
	};

	/**
	 * @struct Animation
	 * @brief Frame‑based animation data and playback state.
	 */
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

	/**
	 * @struct Particle
	 * @brief Individual particle state (position, velocity, colour, size, etc.).
	 */
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

		float rotation = 0;
		float angularVelocity = 0;

		Texture2D texture = {};
		Rectangle texRect = {};
	};

	/**
	 * @struct ParticleEmitter
	 * @brief Spawn parameters and runtime state for a particle emitter.
	 */
	struct ParticleEmitter
	{
		float radius = 1;
		bool edgeOnly = false;

		float spawnRate = 10;
		u32 burst = 0;
		bool playing = true;

		float lifetimeMin = 1;
		float lifetimeMax = 1;

		float speedMin = 1;
		float speedMax = 1;

		bool useDirection = false;
		float directionAngle = 0;
		float directionSpread = 0;

		Color startColor = WHITE;
		Color endColor = {255, 255, 255, 0};
		float startSize = 1;
		float endSize = 1;

		Texture2D texture = {};
		std::vector<Rectangle> textureFrames;

		float angularVelocityMin = 0;
		float angularVelocityMax = 0;
		float initialRotationMin = 0;
		float initialRotationMax = 0;

		std::function<Component::Particle(const Component::ParticleEmitter&, const Vec2<float>&)> spawnOverride;

		float gravity = 0;

		float spawnAccumulator = 0;
	};

	/**
	 * @struct NetworkId
	 * @brief Associates a local entity with a remote owner and remote entity ID.
	 */
	struct NetworkId
	{
		UUID owner = NULL_UUID;
		Entity remoteEntity = NULL_ENTITY;
	};
}