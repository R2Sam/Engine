#pragma once

#include "Types.h"

#include "LuaManager.h"
#include "NetworkManager.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include "SceneManager.h"
#include "SystemManager.h"
#include "entt/entt.h"

#include <string>

using Entity = entt::entity;
using Registry = entt::registry;
using Dispatcher = entt::dispatcher;

namespace Event
{
	/**
	 * @brief Event signalling that the game should close
	 */

	struct CloseGame
	{
		/**
		 * @brief Registers the event type with Lua
		 *
		 * This is called by lua when registering an even with the lua manager.
		 *
		 * @param lua Lua state to register into
		 */

		static void LuaRegister(sol::state& lua)
		{
			Lua::RegisterType<Event::CloseGame>(lua, DemangleWithoutNamespace<Event::CloseGame>().c_str(),
			sol::constructors<CloseGame()>());
		}
	};
}

struct WindowInfo
{
	u32 width = 1080;
	u32 height = 720;
	std::string title;

	bool fullscreen = false;
	bool borderlessFullscreen = false;
	bool resizable = false;
	bool undecorated = false;
	bool topmost = false;
	bool alwaysRun = false;
	bool transparent = false;
	bool highDpi = false;
	bool msaa4x = false;
};

/**
 * @brief Core engine
 *
 * Owns and coordinates all major subsystems.
 *
 * Only a single Engine instance may exist at a time.
 */

class Engine
{
public:

	Engine(const WindowInfo& windowInfo);
	~Engine();

	template <typename T, typename... Args>
	void SetFirstScene(const char* name, Args&&... args)
	{
		m_sceneManager.AddScene<T>(name, std::forward<Args>(args)...);
		m_sceneManager.ChangeScene(name);
	}

	void Run(const u32 targetFps, const u32 updateFrequency, const u8 maxUpdatesPerFrame = 5);

	double GetUpdateTime() const;
	double GetDrawTime() const;

	static Engine& Get();

	Registry& registry = m_registry;
	Dispatcher& dispatcher = m_dispatcher;

	// Core systems
	Renderer& renderer = m_renderer;
	ResourceManager& resourceManager = m_resourceManager;
	SceneManager& sceneManager = m_sceneManager;
	SystemManager& systemManager = m_systemManager;
	LuaManager& luaManager = m_luaManager;
	NetworkManager& networkManager = m_networkManager;

private:

	static void SetFlags(const WindowInfo& windowInfo);

	// Event handling
	void OnCloseGameEvent(const Event::CloseGame& event);

	// Ecs
	Registry m_registry;
	Dispatcher m_dispatcher;

	// Core systems
	Renderer m_renderer;
	ResourceManager m_resourceManager;
	SceneManager m_sceneManager;
	SystemManager m_systemManager;
	LuaManager m_luaManager;
	NetworkManager m_networkManager;

	// Timers
	double m_updateTime = 0;
	double m_drawTime = 0;

	bool m_running = true;
};