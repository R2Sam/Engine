#pragma once

#include "Engine/Components.hpp"
#include "Engine/Registry.hpp"
#include "Engine/SystemManager.hpp"

/**
 * @file ParticleSystem.hpp
 * @brief Particle and emitter management.
 */

/**
 * @class ParticleSystem
 * @brief Updates and draws particles emitted by entities.
 *
 * Requires entities to have a Component::ParticleEmitter and a
 * std::vector<Component::Particle>. Handles spawning, physics,
 * aging, and texture‑sorted rendering.
 */
class ParticleSystem : public System
{
public:

	/**
	 * @brief Constructs the system and registers component callbacks.
	 */
	ParticleSystem();

	/**
	 * @brief Updates all active particles and spawns new ones.
	 * @param deltaT Time since last update (seconds).
	 *
	 * - Applies velocity, gravity, angular velocity and aging.
	 * - Removes expired particles.
	 * - If emitter is playing, spawns particles at the given rate.
	 * - Marks sort dirty when particles change.
	 */
	void Update(const float deltaT) override;

	/**
	 * @brief Renders all visible particles.
	 *
	 * Interpolates color and size based on age/lifetime.
	 * Uses DrawTexturePro if a valid texture is provided, otherwise falls back to circles.
	 */
	void Draw() const override;

	/**
	 * @brief Instantly spawns a burst of particles.
	 * @param entity Entity holding the ParticleEmitter.
	 * @param count Number of particles to spawn.
	 *
	 * Does nothing if entity lacks ParticleEmitter or Transform.
	 */
	static void Burst(const Entity entity, u32 count);

	/**
	 * @brief Stops automatic spawning on the emitter.
	 * @param entity Entity with ParticleEmitter.
	 */
	static void Stop(const Entity entity);

	/**
	 * @brief Resumes automatic spawning on the emitter.
	 * @param entity Entity with ParticleEmitter.
	 */
	static void Play(const Entity entity);

private:

	static void SpawnParticle(const Entity entity, const Component::ParticleEmitter& emitter,
	const Vec2<float>& worldPos);

	void MarkNeedSort();

	bool m_needSort = false;
};