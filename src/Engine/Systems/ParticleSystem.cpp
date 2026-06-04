#include "ParticleSystem.hpp"

#include "Engine/Components.hpp"
#include "Engine/Engine.hpp"

#include "Utils/RaylibUtils.hpp"
#include "raylib.h"
#include <execution>
#include <random>

static float RandFloat(const float min, const float max);
static float RandAngleDeg();
static Color LerpColor(const Color& a, const Color& b, const float t);

ParticleSystem::ParticleSystem()
{
	REGISTRY.OnConstruct<Component::Particle>([this](Component::Particle&, const Entity)
	{
		MarkNeedSort();
	});

	REGISTRY.OnUpdate<Component::Particle>([this](Component::Particle&, const Entity)
	{
		MarkNeedSort();
	});

	REGISTRY.OnDestroy<Component::Particle>([this](Component::Particle&, const Entity)
	{
		MarkNeedSort();
	});
}

void ParticleSystem::Update(const float deltaT) // NOLINT
{
	auto view = REGISTRY.GetView<std::vector<Component::Particle>, Component::ParticleEmitter>();

	for (auto [entity, particles, emitter] : view.each())
	{
		auto particlesCopy = particles;
		auto emitterCopy = emitter;

		bool particlesDirty = false;
		bool emitterDirty = false;

		for (auto it = particlesCopy.begin(); it != particlesCopy.end();)
		{
			it->age += deltaT;
			it->rotation += it->angularVelocity * deltaT;
			it->velocity.y += emitterCopy.gravity * deltaT;
			it->position.x += it->velocity.x * deltaT;
			it->position.y += it->velocity.y * deltaT;

			if (it->age >= it->lifetime)
			{
				it = particlesCopy.erase(it);
				particlesDirty = true;
			}
			else
			{
				++it;
			}
		}

		if (emitterCopy.playing && emitterCopy.spawnRate > 0)
		{
			const auto* transform = REGISTRY.Get<Component::Transform>(entity);
			if (transform)
			{
				emitterCopy.spawnAccumulator += emitterCopy.spawnRate * deltaT;
				emitterDirty = true;

				while (emitterCopy.spawnAccumulator >= 1)
				{
					SpawnParticle(entity, emitterCopy, transform->position);
					emitterCopy.spawnAccumulator -= 1;

					const auto* updated = REGISTRY.Get<std::vector<Component::Particle>>(entity);
					if (updated)
					{
						particlesCopy = *updated;
					}
				}

				particlesDirty = true;
			}
		}

		if (particlesDirty)
		{
			REGISTRY.Replace<std::vector<Component::Particle>>(entity, particlesCopy);
		}

		if (emitterDirty)
		{
			REGISTRY.Replace<Component::ParticleEmitter>(entity, emitterCopy);
		}
	}

	if (m_needSort)
	{
		REGISTRY.Sort<Component::Particle>([](const Component::Particle& a, const Component::Particle& b)
		{
			return a.texture.id < b.texture.id;
		});

		m_needSort = false;
	}
}

void ParticleSystem::Draw() const
{
	auto view = REGISTRY.GetView<std::vector<Component::Particle>>();

	BeginMode2D(RENDERER.camera);

	for (auto [entity, particles] : view.each())
	{
		for (const auto& particle : particles)
		{
			const float t = (particle.lifetime > 0) ? (particle.age / particle.lifetime) : 1;
			const Color color = LerpColor(particle.startColor, particle.endColor, t);
			const float size = particle.startSize + ((particle.endSize - particle.startSize) * t);

			if (size <= 0.f || color.a == 0)
			{
				continue;
			}

			if (IsTextureValid(particle.texture) &&
				IsTextureVisible(particle.texture, 1, particle.position.Raylib(), RENDERER.camera))
			{
				const float halfW = (particle.texRect.width * size) * 0.5;
				const float halfH = (particle.texRect.height * size) * 0.5;

				Rectangle dest = {particle.position.x, particle.position.y, halfW * 2, halfH * 2};
				Vector2 origin = {halfW, halfH};

				DrawTexturePro(particle.texture, particle.texRect, dest, origin, particle.rotation, color);
			}

			else
			{
				if (IsCircleVisible(size, particle.position.Raylib(), RENDERER.camera))
				{
					DrawCircleV(particle.position.Raylib(), size, color);
				}
			}
		}
	}

	EndMode2D();
}

void ParticleSystem::Burst(const Entity entity, const u32 count)
{
	const auto* emitter = REGISTRY.Get<Component::ParticleEmitter>(entity);
	const auto* transform = REGISTRY.Get<Component::Transform>(entity);

	if (!emitter || !transform)
	{
		return;
	}

	for (u32 i = 0; i < count; ++i)
	{
		SpawnParticle(entity, *emitter, transform->position);
	}
}

void ParticleSystem::Stop(const Entity entity)
{
	REGISTRY.Patch<Component::ParticleEmitter>(entity, [](Component::ParticleEmitter& e)
	{
		e.playing = false;
	});
}

void ParticleSystem::Play(const Entity entity)
{
	REGISTRY.Patch<Component::ParticleEmitter>(entity, [](Component::ParticleEmitter& e)
	{
		e.playing = true;
	});
}

void ParticleSystem::SpawnParticle(const Entity entity, const Component::ParticleEmitter& emitter,
const Vec2<float>& worldPos)
{

	Vec2<float> spawnPos = worldPos;

	float angle = RandAngleDeg() * DEG2RAD;

	float r = emitter.edgeOnly ? emitter.radius : emitter.radius * std::sqrt(RandFloat(0, 1));

	spawnPos.x += r * std::cos(angle);
	spawnPos.y += r * std::sin(angle);

	float angleDeg = emitter.useDirection ? RandFloat(emitter.directionAngle - emitter.directionSpread,
											emitter.directionAngle + emitter.directionSpread)
										  : RandAngleDeg();

	float angleRad = angleDeg * DEG2RAD;
	float speed = RandFloat(emitter.speedMin, emitter.speedMax);

	Vec2<float> velocity{speed * std::cos(angleRad), speed * std::sin(angleRad)};

	Component::Particle particle;
	particle.position = spawnPos;
	particle.velocity = velocity;
	particle.lifetime = RandFloat(emitter.lifetimeMin, emitter.lifetimeMax);
	particle.age = 0.f;
	particle.startColor = emitter.startColor;
	particle.endColor = emitter.endColor;
	particle.startSize = emitter.startSize;
	particle.endSize = emitter.endSize;

	if (!REGISTRY.HasAny<std::vector<Component::Particle>>(entity))
	{
		REGISTRY.Emplace<std::vector<Component::Particle>>(entity);
	}

	REGISTRY.Patch<std::vector<Component::Particle>>(entity, [&particle](std::vector<Component::Particle>& particles)
	{
		particles.push_back(particle);
	});
}

void ParticleSystem::MarkNeedSort()
{
	m_needSort = true;
}

static float RandFloat(const float min, float max)
{
	static std::random_device s_rd;
	static std::mt19937 s_gen(s_rd());
	std::uniform_real_distribution<float> dist(min, max);

	return dist(s_gen);
}

static float RandAngleDeg()
{
	return RandFloat(0, 360);
}

static Color LerpColor(const Color& a, const Color& b, const float t)
{
	return Color{
	static_cast<unsigned char>(a.r + ((b.r - a.r) * t)),
	static_cast<unsigned char>(a.g + ((b.g - a.g) * t)),
	static_cast<unsigned char>(a.b + ((b.b - a.b) * t)),
	static_cast<unsigned char>(a.a + ((b.a - a.a) * t)),
	};
}