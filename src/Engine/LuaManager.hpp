#pragma once

#include "Lua/MyLua.hpp"

#include "entt/entt.h"

#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>

struct LuaScript
{
	sol::environment environment;
	bool enabled = true;
};

class LuaManager
{
public:

	template <typename Event>
	void RegisterRecieveEvent(entt::dispatcher& dispatcher)
	{
		if (!Lua::TypeExists<Event>(lua))
		{
			Event event;
			event.LuaRegister(lua);
		}

		dispatcher.sink<Event>().template connect<&LuaManager::OnEvent<Event>>(this);
	}

	template <typename Event>
	void RegisterSendEvent(entt::dispatcher& dispatcher)
	{
		if (!Lua::TypeExists<Event>(lua))
		{
			Event event;
			event.LuaRegister(lua);
		};

		std::string functionName = "Trigger" + DemangleWithoutNamespace<Event>() + "Event";

		if (!Lua::FunctionExists(lua, functionName.c_str()))
		{
			Lua::RegisterFunction(lua, functionName.c_str(), [this, &dispatcher](Event event)
			{
				dispatcher.trigger<Event>(std::move(event));
			});
		}
	}

	bool LoadScript(const std::string& path);
	void RemoveScript(const std::string& path);

	bool IsScriptLoaded(const std::string& path);

	void EnableScript(const std::string& path);
	void DisableScript(const std::string& path);

	std::optional<sol::environment> GetScriptEnvironment(const std::string& path);

	void ReloadScripts();

	sol::state lua;

private:

	LuaManager();

	void Update(const float deltaT);

	template <typename Event>
	void OnEvent(const Event& event)
	{
		std::unique_lock lock(m_mutex);

		for (auto& [path, script] : m_scripts)
		{
			if (script.enabled)
			{
				std::string functionName = "On" + DemangleWithoutNamespace<Event>() + "Event";
				Lua::CallFunction<false>(script.environment, functionName.c_str(), event);
			}
		}
	}

	std::mutex m_mutex;

	std::unordered_map<std::string, LuaScript> m_scripts;

	friend class Engine;
};