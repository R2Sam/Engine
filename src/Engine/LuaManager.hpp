#pragma once

#include "Lua/MyLua.hpp"

#include "entt/entt.hpp"

#include <functional>
#include <mutex>
#include <optional>
#include <queue>
#include <string>
#include <unordered_map>

struct LuaScript
{
	sol::environment environment;
	std::string directory;
	bool enabled = true;
};

class LuaManager
{
public:

	template <typename Event>
	void RegisterReceiveEvent(entt::dispatcher& dispatcher)
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
				std::unique_lock lock(m_mutex);
				m_pendingEvents.push([&dispatcher, e = std::move(event)]() mutable
				{
					dispatcher.trigger<Event>(std::move(e));
				});
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
		// Copy event and queue the dispatch so we don't call into Lua while holding
		// the mutex — avoids deadlock if a script triggers another event.
		std::unique_lock lock(m_mutex);

		for (auto& [path, script] : m_scripts)
		{
			if (script.enabled)
			{
				std::string functionName = "On" + DemangleWithoutNamespace<Event>() + "Event";
				Event copy = event;
				m_pendingEvents.push([&script, functionName, copy]()
				{
					Lua::CallFunction<false>(script.environment, functionName.c_str(), copy);
				});
			}
		}
	}

	std::mutex m_mutex;

	std::unordered_map<std::string, LuaScript> m_scripts;

	std::queue<std::function<void()>> m_pendingEvents;

	friend class Engine;
};