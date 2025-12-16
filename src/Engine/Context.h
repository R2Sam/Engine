#pragma once

#include "entt/entt.h"
#include "ResourceManager.h"
#include "SceneManager.h"
#include "SystemManager.h"
#include "LuaManager.h"
#include "Logger.h"

struct Context
{
	entt::registry& registry;
	entt::dispatcher& dispatcher;
	ResourceManager& resourceManager;
	SceneManager& sceneManager;
	SystemManager& systemManager;
	LuaManager& luaManager;
	Logger& logger;
};

namespace Event
{
	struct CloseGame
	{
		void LuaRegister(sol::state& lua)
		{
			Lua::RegisterType<Event::CloseGame>(lua, DemangleWithoutNamespace<Event::CloseGame>().c_str(),
				sol::constructors<CloseGame()>());
		}
	};
}