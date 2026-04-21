#pragma once

#include "Engine.hpp"
#include "SystemManager.hpp"

class AnimationSystem : public System
{
public:

	AnimationSystem();

	void Update(const float deltaT) override;

	static std::vector<Entity> GetIncompleteAnimations();
	static bool IsAnimationComeplete(const Entity entity);

private:

	static void Check(Registry& registry, Entity entity);
};