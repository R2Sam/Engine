#include "AnimationSystem.hpp"
#include "Engine/Components.hpp"
#include "Engine/Engine.hpp"
#include <vector>

AnimationSystem::AnimationSystem()
{
	REGISTRY.OnConstruct<Component::Animation>([](Component::Animation&, const Entity entity)
	{
		AnimationSystem::Check(REGISTRY, entity);
	});
}

void AnimationSystem::Update([[maybe_unused]] const float deltaT)
{
	auto view = REGISTRY.GetView<Component::Animation, Component::Sprite>();

	for (auto [entity, anim, sprt] : view.each())
	{
		Component::Animation animation = anim;
		Component::Sprite sprite = sprt;

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

		REGISTRY.Replace<Component::Animation>(entity, animation);
		REGISTRY.Replace<Component::Sprite>(entity, sprite);
	}
}

void AnimationSystem::PlayAnimation(const Entity entity, const Component::Animation& animation)
{
	Component::Animation newAnimation = animation;
	newAnimation.active = true;
	newAnimation.restart = true;

	const Component::Animation* oldAnimation = REGISTRY.Get<Component::Animation>(entity);
	if (oldAnimation)
	{
		REGISTRY.Replace<Component::Animation>(entity, newAnimation);
	}

	else
	{
		REGISTRY.Emplace<Component::Animation>(entity, newAnimation);
	}
}

void AnimationSystem::PlayAnimation(const Entity entity)
{
	REGISTRY.Patch<Component::Animation>(entity, [](Component::Animation& animation)
	{
		animation.restart = true;
	});
}

bool AnimationSystem::IsPlaying(const Entity entity)
{
	const Component::Animation* animation = REGISTRY.Get<Component::Animation>(entity);
	return animation && animation->active;
}

std::vector<Entity> AnimationSystem::GetIncompleteAnimations()
{
	auto view = REGISTRY.GetView<Component::Animation>();

	std::vector<Entity> results;

	for (auto [entity, animation] : view.each())
	{
		if (animation.active && !animation.loop)
		{
			results.emplace_back(entity);
		}
	}

	return results;
}

void AnimationSystem::Check(Registry& registry, Entity entity)
{
	const Component::Animation* animation = registry.Get<Component::Animation>(entity);

	Assert(registry.HasAny<Component::Sprite>(entity), "Animation must have a sprite");
	const Component::Sprite* sprite = registry.Get<Component::Sprite>(entity);

	u32 rowLength = sprite->texture.width / sprite->rectangle.width;
	u32 rowCount = sprite->texture.height / sprite->rectangle.height;
	u32 totalSprites = rowLength * rowCount;

	Assert(sprite->rectangle.width && sprite->rectangle.height, "Sprite must have a size");
	Assert(animation->startingIndex < totalSprites,
	"Animation starting index must be smaller than total sprite count: ", totalSprites);
	Assert(animation->endingIndex < totalSprites,
	"Animation ending index must be smaller than total sprite count: ", totalSprites);
	Assert(animation->currentIndex >= animation->startingIndex && animation->currentIndex <= animation->endingIndex,
	"Animation starting index must be between starting and ending indices: ", animation->startingIndex, "-",
	animation->endingIndex);
	Assert(animation->frameAccumulator < animation->frameLengthS,
	"Animation frame accumulator must be bellow frame length: ", animation->frameLengthS);
}