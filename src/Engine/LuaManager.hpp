#pragma once

#include "Lua/MyLua.hpp"

#include "entt/entt.hpp"

#include <functional>
#include <mutex>
#include <optional>
#include <queue>
#include <string>
#include <unordered_map>

/**
 * @file LuaManager.hpp
 * @brief Lua sandboxing and scripts.
 */

/**
 * @brief Holds the runtime state of a single Lua script
 */
struct LuaScript
{
	/// Isolated Lua environment; each script gets its own globals table
	sol::environment environment;

	/// Directory the script was loaded from; used to resolve sandboxed requires
	std::string directory;

	/// When false the script's Update function is not called each frame
	bool enabled = true;
};

/**
 * @brief Manages sandboxed Lua scripts and their integration with the event dispatcher
 *
 * Each script runs in its own sol::environment so scripts cannot interfere with
 * each other's globals. Scripts receive a sandboxed require that is restricted to
 * their own directory.
 *
 * Event dispatch into Lua is deferred: events are queued on the calling thread and
 * flushed at the start of the next Update to avoid calling into Lua while holding
 * the internal mutex.
 */
class LuaManager
{
public:

	/**
	 * @brief Connects the dispatcher so all loaded scripts receive events of type Event
	 *
	 * Registers the C++ type with Lua if it has not been registered yet.
	 * Scripts handle the event by defining a global function named
	 * "On<EventTypeName>Event".
	 *
	 * @tparam Event Event type; must expose a static LuaRegister(sol::state&) method
	 * @param dispatcher Dispatcher to listen on
	 */
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

	/**
	 * @brief Exposes a global Lua function that fires an event on the dispatcher
	 *
	 * Registers the C++ type with Lua if it has not been registered yet.
	 * The generated function is named "Trigger<EventTypeName>Event".
	 * The trigger is deferred through the pending-events queue to keep dispatches
	 * on the main thread.
	 *
	 * @tparam Event Event type; must expose a static LuaRegister(sol::state&) method
	 * @param dispatcher Dispatcher to trigger events on
	 */
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

	/**
	 * @brief Loads and executes a Lua script in its own sandboxed environment
	 *
	 * The script is stored under its path so it can be retrieved, reloaded, or removed
	 * later. Returns false if the script is already loaded or fails to execute.
	 *
	 * @param path File path of the script to load
	 * @return True if the script was loaded successfully
	 */
	bool LoadScript(const std::string& path);

	/**
	 * @brief Unloads a script and discards its environment
	 *
	 * Does nothing if the script is not currently loaded.
	 *
	 * @param path File path of the script to remove
	 */
	void RemoveScript(const std::string& path);

	/**
	 * @brief Checks whether a script is currently loaded
	 *
	 * @param path File path to check
	 * @return True if the script has been loaded and not yet removed
	 */
	bool IsScriptLoaded(const std::string& path);

	/**
	 * @brief Allows a script's Update function to be called each frame
	 *
	 * Does nothing if the script is not loaded.
	 *
	 * @param path File path of the script to enable
	 */
	void EnableScript(const std::string& path);

	/**
	 * @brief Prevents a script's Update function from being called each frame
	 *
	 * The script remains loaded and its environment is preserved.
	 * Does nothing if the script is not loaded.
	 *
	 * @param path File path of the script to disable
	 */
	void DisableScript(const std::string& path);

	/**
	 * @brief Returns the sol::environment of a loaded script
	 *
	 * Useful for reading or writing script globals from C++.
	 *
	 * @param path File path of the script
	 * @return The script's environment, or nullopt if it is not loaded
	 */
	std::optional<sol::environment> GetScriptEnvironment(const std::string& path);

	/**
	 * @brief Re-executes all loaded scripts in fresh environments
	 *
	 * Useful for hot-reloading during development. Each script's environment is
	 * replaced and the file is re-run from disk.
	 */
	void ReloadScripts();

	/// The shared Lua state; all scripts execute within this state
	sol::state lua;

private:

	LuaManager();

	/**
	 * @brief Flushes pending events and calls Update on all enabled scripts
	 *
	 * Called once per frame by the Engine before scenes are updated.
	 *
	 * @param deltaT Duration of the previous frame in seconds
	 */
	void Update(const float deltaT);

	/**
	 * @brief Queues an event to be dispatched into all enabled scripts on the next Update
	 *
	 * Copies the event so the original may be destroyed before the queue is flushed.
	 * Scripts handle the event via a global function named "On<EventTypeName>Event".
	 *
	 * @tparam Event Event type
	 * @param event The event to forward to scripts
	 */
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