#pragma once

#include "Engine/Components.hpp"
#include "Engine/Registry.hpp"
#include "Engine/SystemManager.hpp"

class ParticleSystem : public System
{
public:

	void Update(const float deltaT) override;

	void Draw() const override;

	static void Burst(const Entity entity, u32 count);

	static void Stop(const Entity entity);
	static void Play(const Entity entity);

private:

	static void SpawnParticle(const Entity entity, const Component::ParticleEmitter& emitter,
	const Vec2<float>& worldPos);
};