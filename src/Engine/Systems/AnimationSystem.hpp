#pragma once

#include "Engine/Components.hpp"
#include "Engine/Registry.hpp"
#include "Engine/SystemManager.hpp"
#include "raylib.h"

/**
 * @file AnimationSystem.hpp
 * @brief Animation managing and updating.
 */

/**
 * @class AnimationSystem
 * @brief Drives Component::Animation and updates Component::Sprite accordingly.
 */
class AnimationSystem : public System
{
public:

	/**
	 * @brief Updates all animations.
	 * @param deltaT Time since last update (seconds).
	 *
	 * Advances the animation time, calculates the current frame,
	 * and updates the sprite's texture rectangle.
	 */
	void Update(const float deltaT) override;

	/**
	 * @brief Creates an Animation component from a sprite sheet grid.
	 * @param texture Sprite sheet texture.
	 * @param cellWidth Width of each frame in pixels.
	 * @param cellHeight Height of each frame in pixels.
	 * @param startIndex First frame index (row‑major order).
	 * @param endIndex Last frame index (inclusive).
	 * @param duration Seconds per frame.
	 * @param loop True to repeat the animation.
	 * @return A fully initialised Animation component.
	 *
	 * @note Asserts if texture invalid, cell sizes zero, or index range is invalid.
	 */
	static Component::Animation GridAnimation(const Texture2D texture, const u32 cellWidth, const u32 cellHeight,
	const u32 startIndex, u32 endIndex, const float duration, const bool loop);

	/**
	 * @brief Starts playing an animation on an entity.
	 * @param entity Target entity.
	 * @param animation The animation data to play.
	 *
	 * Replaces any existing Animation component. If no Sprite component exists,
	 * one is created using the first frame of the animation.
	 */
	static void Play(const Entity entity, const Component::Animation& animation);

	/**
	 * @brief Pauses the animation.
	 * @param entity Entity with an Animation component.
	 */
	static void Stop(const Entity entity);

	/**
	 * @brief Resumes a paused animation.
	 * @param entity Entity with an Animation component.
	 */
	static void Resume(const Entity entity);

	/**
	 * @brief Checks whether an animation is currently playing.
	 * @param entity Entity with an Animation component.
	 * @return True if animation exists, is playing, and has at least one frame.
	 */
	static bool IsPlaying(const Entity entity);

	/**
	 * @brief Sets the playback speed multiplier.
	 * @param entity Entity with an Animation component.
	 * @param speed Speed factor (1.0 = normal, 2.0 = double speed).
	 */
	static void SetSpeed(const Entity entity, float speed);
};