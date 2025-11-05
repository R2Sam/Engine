#pragma once

#include "entt/entt.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include "SceneManager.h"
#include "SystemManager.h"
#include "LuaManager.h"

struct Context
{
	entt::registry& registry;
	entt::dispatcher& dispatcher;
	Renderer& renderer;
	ResourceManager& resourceManager;
	SceneManager& sceneManager;
	SystemManager& systemManager;
	LuaManager& luaManager;
};

namespace Event
{
	struct CloseGame
	{

	};
}