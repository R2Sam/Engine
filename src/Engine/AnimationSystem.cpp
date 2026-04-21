#include "AnimationSystem.hpp"
#include "Components.hpp"
#include "Engine/Engine.hpp"
#include <vector>

AnimationSystem::AnimationSystem()
{
	REGISTRY.on_construct<Component::Animation>().connect<&AnimationSystem::Check>();
}

void AnimationSystem::Update(const float deltaT)
{
	auto group = REGISTRY.group<Component::Animation>(entt::get<Component::Sprite>);

	for (auto [entity, animation, sprite] : group.each())
	{
		if (animation.restart)
		{
			animation.currentIndex = animation.startingIndex;
			animation.frameAccumulator = 0;
			animation.active = true;
			animation.restart = false;
		}

		if (!animation.active)
		{
			continue;
		}

		u32 rowLength = sprite.texture.width / sprite.rectangle.width;
		u32 rowCount = sprite.texture.height / sprite.rectangle.height;
		u32 totalSprites = rowLength * rowCount;

		if (animation.frameAccumulator >= animation.frameLengthS)
		{
			animation.frameAccumulator -= animation.frameLengthS;

			++animation.currentIndex;

			if (animation.currentIndex > animation.endingIndex)
			{
				if (animation.loop)
				{
					animation.currentIndex = animation.startingIndex;
				}

				else
				{
					animation.currentIndex = animation.endingIndex;
					animation.active = false;
				}
			}

			if (animation.currentIndex >= totalSprites)
			{
				animation.currentIndex = 0;
			}
		}

		Vec2<u32> gridPosition = {animation.currentIndex % rowLength, animation.currentIndex / rowLength};

		sprite.rectangle = {gridPosition.x * sprite.rectangle.width, gridPosition.y * sprite.rectangle.height,
		sprite.rectangle.width, sprite.rectangle.height};

		animation.frameAccumulator += deltaT;
	}
}

std::vector<Entity> AnimationSystem::GetIncompleteAnimations()
{
	auto group = REGISTRY.group<Component::Animation>(entt::get<Component::Sprite>);

	std::vector<Entity> results;

	for (auto [entity, animation, sprite] : group.each())
	{
		if (animation.active && !animation.loop)
		{
			results.emplace_back(entity);
		}
	}

	return results;
}

bool AnimationSystem::IsAnimationComplete(const Entity entity)
{
	Component::Animation* animation = REGISTRY.try_get<Component::Animation>(entity);
	if (!animation)
	{
		return true;
	}

	if (animation->restart)
	{
		return false;
	}

	if (animation->active)
	{
		return animation->loop;
	}

	return true;
}

void AnimationSystem::Check(Registry& registry, Entity entity)
{
	const Component::Animation& animation = registry.get<Component::Animation>(entity);

	Assert(registry.any_of<Component::Sprite>(entity), "Animation must have a sprite");
	const Component::Sprite& sprite = registry.get<Component::Sprite>(entity);

	u32 rowLength = sprite.texture.width / sprite.rectangle.width;
	u32 rowCount = sprite.texture.height / sprite.rectangle.height;
	u32 totalSprites = rowLength * rowCount;

	Assert(sprite.rectangle.width && sprite.rectangle.height, "Sprite must have a size");
	Assert(animation.startingIndex < totalSprites,
	"Animation starting index must be smaller than total sprite count: ", totalSprites);
	Assert(animation.endingIndex < totalSprites,
	"Animation ending index must be smaller than total sprite count: ", totalSprites);
	Assert(animation.currentIndex >= animation.startingIndex && animation.currentIndex <= animation.endingIndex,
	"Animation starting index must be between starting and ending indices: ", animation.startingIndex, "-",
	animation.endingIndex);
	Assert(animation.frameAccumulator < animation.frameLengthS,
	"Animation frame accumulator must be bellow frame length: ", animation.frameLengthS);
}