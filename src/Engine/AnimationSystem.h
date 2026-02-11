#pragma once

#include "SystemManager.h"

#include "entt/entt.h"

class AnimationSystem : public System
{
public:

	AnimationSystem();

	void Update(const float deltaT) override;
	void Draw() override;

private:

	static void Check(entt::registry& registry, entt::entity entity);
};