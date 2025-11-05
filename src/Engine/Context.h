#pragma once

#include "entt/entt.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include "SceneManager.h"
#include "SystemManager.h"

struct Context
{
	entt::registry& registry;
	entt::dispatcher& dispatcher;
	Renderer& renderer;
	ResourceManager& resourceManger;
	SceneManager& sceneManager;
	SystemManager& systemManager;
};

namespace Event
{
	struct CloseGame
	{

	};
}