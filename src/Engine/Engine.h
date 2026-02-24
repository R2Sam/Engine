#pragma once

#include "Types.h"

#include "LuaManager.h"
#include "NetworkManager.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include "SceneManager.h"
#include "SystemManager.h"
#include "entt/entt.h"
#include "raylib.h"

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
		 * This is called by lua when registering an event with the lua manager.
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

/**
 * @brief Initial window info
 *
 * Tells engine what the initial window size should be and which flags to enable
 */

struct WindowInfo
{
	u32 width = 1280;
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

	/**
	 * @ brief Creates the engine and all its systems
	 *
	 * @param windowInfo Initial window info struct
	 */

	Engine(const WindowInfo& windowInfo);
	~Engine();

	/**
	 * @brief Sets the initial scene
	 *
	 * @tparam T Scene type
	 * @tparam Args T Constructor argument types
	 * @param args Scene constructor arguments
	 */

	template <typename T, typename... Args>
	void SetFirstScene(Args&&... args)
	{
		m_sceneManager.AddScene<T>(std::forward<Args>(args)...);
		m_sceneManager.ChangeScene<T>();
	}

	/**
	 * @brief Runs the engine loop
	 *
	 * This is blocking until the widow is closed or a CloseGame event is called.
	 *
	 * @param targetFps The target fps for the engine to run at
	 * @param updateFrequency How many update loops should be run per second
	 * @param maxUpdatesPerFrame Maximum updated per frame when the engine is trying to catch up
	 */

	void Run(const u32 targetFps, const u32 updateFrequency, const u8 maxUpdatesPerFrame = 5);

	/**
	 * @brief Returns how long the average update loop took in ms
	 */

	double GetUpdateTime() const;

	/**
	 * @brief Returns how long the average draw loop took in ms
	 */

	double GetDrawTime() const;

	/**
	 * @brief Returns the engine instance
	 *
	 * This is mainly to be used to access the public systems.
	 * If an engine hasn't been instanced the internal assert will fail.
	 */

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

	// Mouse
	Vector2 mVirtualMousePos = {};

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

	// Canvas
	RenderTexture2D m_canvas;

	bool m_running = true;
};