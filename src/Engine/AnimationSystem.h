#pragma once

#include "SystemManager.h"

#include "entt/entt.h"

class AnimationSystem : public System
{
public:

	AnimationSystem(const Context& context);

	void Update(const float deltaT) override;
	void Draw() override;

private:

	void Check(entt::registry& registry, entt::entity entity);
};