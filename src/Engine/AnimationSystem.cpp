#include "AnimationSystem.h"

#include "Components.h"

#include "Engine.h"

AnimationSystem::AnimationSystem()
{
	Engine::Get().registry.on_construct<Component::Animation>().connect<&AnimationSystem::Check>();
}

void AnimationSystem::Update(const float deltaT)
{
	auto group = Engine::Get().registry.group<Component::Animation>(entt::get<Component::Sprite>);

	for (auto [entity, animation, sprite] : group.each())
	{
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
				animation.currentIndex = animation.startingIndex;
			}

			if (animation.currentIndex >= totalSprites)
			{
				animation.currentIndex = 0;
			}
		}

		Vector2i gridPosition = {animation.currentIndex % rowLength, animation.currentIndex / rowLength};

		sprite.rectangle = {gridPosition.x * sprite.rectangle.width, gridPosition.y * sprite.rectangle.height,
		sprite.rectangle.width, sprite.rectangle.height};

		animation.frameAccumulator += deltaT;
	}
}

void AnimationSystem::Draw()
{
}

void AnimationSystem::Check(entt::registry& registry, entt::entity entity)
{
	const Component::Animation& animation = registry.get<Component::Animation>(entity);

	Assert(registry.any_of<Component::Sprite>(entity), "Animation must have a sprite");
	const Component::Sprite& sprite = registry.get<Component::Sprite>(entity);

	u32 rowLength = sprite.texture.width / sprite.rectangle.width;
	u32 rowCount = sprite.texture.height / sprite.rectangle.height;
	u32 totalSprites = rowLength * rowCount;

	Assert(sprite.rectangle.width && sprite.rectangle.height, "Sprite must have a size");
	Assert(animation.startingIndex < totalSprites, "Animation starting index must be smaller than total sprite count");
	Assert(animation.endingIndex < totalSprites, "Animation ending index must be smaller than total sprite count");
	Assert(animation.currentIndex >= animation.startingIndex && animation.currentIndex <= animation.endingIndex,
	"Animation starting index must be between starting and ending indices");
	Assert(animation.frameAccumulator < animation.frameLengthS,
	"Animation frame accumulator must be bellow frame length");
}