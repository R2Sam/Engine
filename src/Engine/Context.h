#pragma once

#include "entt/entt.h"
#include "ResourceManager.h"
#include "SceneManager.h"
#include "SystemManager.h"
#include "LuaManager.h"
#include "NetworkManager.h"
#include "Logger.h"

using Entity = entt::entity;
using Registry = entt::registry;
using Dispatcher = entt::dispatcher;

#define LogDebug(...) do { _context.logger.Write(LogLevel::DEBUG, __VA_ARGS__); } while(0)

struct Context
{
	entt::registry& registry;
	entt::dispatcher& dispatcher;
	ResourceManager& resourceManager;
	SceneManager& sceneManager;
	SystemManager& systemManager;
	LuaManager& luaManager;
	NetworkManager& networkManager;
	Logger& logger;

	double updateTime;
	double drawTime; 
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