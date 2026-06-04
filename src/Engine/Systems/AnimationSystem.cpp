#include "AnimationSystem.hpp"

#include "Engine/Components.hpp"
#include "Engine/Engine.hpp"
#include "raylib.h"
#include <cmath>

void AnimationSystem::Update([[maybe_unused]] const float deltaT)
{
	auto view = REGISTRY.GetView<Component::Animation, Component::Sprite>();

	for (auto [entity, anim, sprt] : view.each())
	{
		auto animation = anim;
		auto sprite = sprt;

		if (!animation.playing || animation.frames.empty())
		{
			continue;
		}

		float newTime = animation.time + (deltaT * animation.speed);
		float totalDuration = animation.frames.size() * animation.frameDuration;

		if (!animation.loop && newTime >= totalDuration)
		{
			newTime = totalDuration - 0.001f;
			animation.playing = false;
		}

		else if (animation.loop)
		{
			newTime = std::fmod(newTime, totalDuration);
		}

		animation.time = newTime;

		int frameIndex = (int)(animation.time / animation.frameDuration) % animation.frames.size();
		const Rectangle& frameRect = animation.frames[frameIndex];

		sprite.texture = animation.texture;
		sprite.rectangle = frameRect;

		REGISTRY.Replace<Component::Animation>(entity, animation);
		REGISTRY.Replace<Component::Sprite>(entity, sprite);
	}
}

Component::Animation AnimationSystem::GridAnimation(const Texture2D texture, const u32 cellWidth, const u32 cellHeight,
const u32 startIndex, u32 endIndex, const float duration, const bool loop)
{
	Assert(IsTextureValid(texture), "Invalid texture");
	Assert(cellWidth > 0 && cellHeight > 0, "Invalid cell size");

	u32 cols = texture.width / cellWidth;
	u32 rows = texture.height / cellHeight;
	u32 total = cols * rows;

	if (endIndex >= total)
	{
		endIndex = total - 1;
	}

	Assert(startIndex <= endIndex, "Invalid frame range");

	Component::Animation animation;
	animation.texture = texture;
	animation.frameDuration = duration;
	animation.loop = loop;
	animation.frames.reserve(endIndex - startIndex + 1);
	animation.playing = true;
	animation.time = 0;
	animation.speed = 1;

	for (u32 index = startIndex; index <= endIndex; index++)
	{
		u32 x = (index % cols) * cellWidth;
		u32 y = (index / cols) * cellHeight;
		animation.frames.emplace_back(x, y, cellWidth, cellHeight);
	}

	return animation;
}

void AnimationSystem::Play(const Entity entity, const Component::Animation& animation)
{
	Component::Animation newAnimation = animation;

	newAnimation.time = 0;
	newAnimation.playing = true;

	REGISTRY.EmplaceOrReplace<Component::Animation>(entity, newAnimation);

	if (!REGISTRY.HasAny<Component::Sprite>(entity) && !newAnimation.frames.empty())
	{
		REGISTRY.Emplace<Component::Sprite>(entity, newAnimation.texture, newAnimation.frames[0], WHITE, 1, 1);
	}
}

void AnimationSystem::Stop(const Entity entity)
{
	if (const auto* anim = REGISTRY.Get<Component::Animation>(entity))
	{
		auto animation = *anim;
		animation.playing = false;
		REGISTRY.Replace<Component::Animation>(entity, animation);
	}
}

void AnimationSystem::Resume(const Entity entity)
{
	if (const auto* anim = REGISTRY.Get<Component::Animation>(entity))
	{
		auto animation = *anim;
		animation.playing = true;
		REGISTRY.Replace<Component::Animation>(entity, animation);
	}
}

bool AnimationSystem::IsPlaying(const Entity entity)
{
	const auto* animation = REGISTRY.Get<Component::Animation>(entity);
	return animation && animation->playing && !animation->frames.empty();
}

void AnimationSystem::SetSpeed(const Entity entity, float speed)
{
	if (const auto* anim = REGISTRY.Get<Component::Animation>(entity))
	{
		auto animation = *anim;
		animation.speed = speed;
		REGISTRY.Replace<Component::Animation>(entity, animation);
	}
}