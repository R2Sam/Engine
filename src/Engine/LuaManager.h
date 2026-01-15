#pragma once

// Forward
struct Context;

#include "Lua/MyLua.h"

#include "entt/entt.h"

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

	LuaManager();

	void Update(const float deltaT);

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
			Lua::RegisterFunction(lua, functionName.c_str(),
			[this, &dispatcher](Event event)
			{
				dispatcher.trigger<Event>(std::move(event));
			});
		}
	}

	bool LoadScript(const char* path);
	void RemoveScript(const char* path);

	bool IsScriptLoaded(const char* path);

	void EnableScript(const char* path);
	void DisableScript(const char* path);

	std::optional<sol::environment> GetScriptEnvironment(const char* path);

	void ReloadScripts();

	void SetContext(Context& context);

public:

	sol::state lua;

private:

	template <typename Event>
	void OnEvent(const Event& event)
	{
		for (auto& [path, script] : _scripts)
		{
			if (script.enabled)
			{
				std::string functionName = "On" + DemangleWithoutNamespace<Event>() + "Event";
				Lua::CallFunction<false>(script.environment, functionName.c_str(), event);
			}
		}
	}

private:

	Context* _context;

	std::unordered_map<std::string, LuaScript> _scripts;
};