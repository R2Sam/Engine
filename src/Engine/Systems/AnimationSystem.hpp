#pragma once

#include "Engine/Components.hpp"
#include "Engine/Registry.hpp"
#include "Engine/SystemManager.hpp"
#include "raylib.h"

class AnimationSystem : public System
{
public:

	void Update(const float deltaT) override;

	static Component::Animation GridAnimation(const Texture2D texture, const u32 cellWidth, const u32 cellHeight,
	const u32 startIndex, u32 endIndex, const float duration, const bool loop);

	static void Play(const Entity entity, const Component::Animation& animation);
	static void Stop(const Entity entity);
	static void Resume(const Entity entity);
	static bool IsPlaying(const Entity entity);

	static void SetSpeed(const Entity entity, float speed);

private:
};