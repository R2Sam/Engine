#pragma once

#include "Engine.hpp"
#include "Engine/Components.hpp"
#include "SystemManager.hpp"

class AnimationSystem : public System
{
public:

	AnimationSystem();

	void Update(const float deltaT) override;

	static void PlayAnimation(const Entity entity, const Component::Animation& anim);
	static void PlayAnimation(const Entity entity);
	static bool IsPlaying(const Entity entity);

	static std::vector<Entity> GetIncompleteAnimations();

private:

	static void Check(Registry& registry, Entity entity);
};