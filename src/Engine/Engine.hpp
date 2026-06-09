#pragma once

#include "NonCopyable.hpp"
#include "Types.hpp"

#include "Events.hpp"
#include "LuaManager.hpp"
#include "Registry.hpp"
#include "Renderer.hpp"
#include "ResourceManager.hpp"
#include "SceneManager.hpp"
#include "SystemManager.hpp"
#include "entt/entt.hpp"
#include "raylib.h"

#include "Systems/AudioSystem.hpp"
#include "Systems/InputSystem.hpp"

#ifndef __EMSCRIPTEN__
#include "Networking/AsyncNetwork.hpp"
#include "bsThreadPool/BS_thread_pool.hpp"
#endif

#include <string>

/**
 * @file Engine.hpp
 * @brief Core engine.
 */

using Dispatcher = entt::dispatcher;

#define REGISTRY Engine::Get().registry
#define DISPATCHER Engine::Get().dispatcher

// Core
#define RENDERER Engine::Get().renderer
#define RESOURCE_MANAGER Engine::Get().resourceManager
#define SCENE_MANAGER Engine::Get().sceneManager
#define SYSTEM_MANAGER Engine::Get().systemManager
#define LUA_MANAGER Engine::Get().luaManager

#ifndef __EMSCRIPTEN__
#define NETWORK Engine::Get().network
#define THREAD_POOL Engine::Get().threadPool
#endif

// Systems
#define INPUT_SYSTEM SYSTEM_MANAGER.GetSystem<InputSystem>()
#define AUDIO_SYSTEM SYSTEM_MANAGER.GetSystem<AudioSystem>()

/**
 * @brief Initial window info
 *
 * Tells engine what the initial window size should be and which flags to enable.
 *
 * Additionally a virtual window size can be specified to which all elements are drawn.
 * The virtual window is then scaled according to the real window size but aspect ratio is preserved.
 */
struct WindowInfo
{
	u32 width = 1280;
	u32 height = 720;
	std::string title;

	u32 virtualWidth = 1920;
	u32 virtualHeight = 1080;

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
 * Only a single Engine instance may exist at a time. Use Engine::Get() to access
 * the instance, or prefer the convenience macros (REGISTRY, DISPATCHER, RENDERER,
 * etc.) which call through to it.
 */
class Engine : public NonCopyable<>
{
public:

	/**
	 * @brief Creates the engine and all its subsystems
	 *
	 * Opens a raylib window, initialises the audio device, registers default
	 * resource caches (Texture2D, Image, Sound, Music, Wave, raw text and binary
	 * files), and adds the built-in systems (AnimationSystem, InputSystem,
	 * AudioSystem, ParticleSystem).
	 *
	 * @param windowInfo Initial window configuration
	 */
	Engine(const WindowInfo& windowInfo);

	~Engine();

	/**
	 * @brief Sets the initial scene and immediately transitions to it
	 *
	 * The scene is constructed with the given arguments, added to the
	 * SceneManager, and made active before Run is called.
	 *
	 * @tparam T Scene type (must derive from Scene)
	 * @tparam Args T constructor argument types
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
	 * This is blocking until the window is closed or a CloseGame event is dispatched.
	 *
	 * Each frame the loop:
	 * -# Accumulates elapsed time and runs fixed-timestep Update passes (systems → Lua → scene)
	 * -# Calls the Renderer update (sprite sort)
	 * -# Draws to the virtual canvas (renderer → systems → scene)
	 * -# Scales the canvas to the real window and presents it
	 *
	 * @param targetFps         Target frames per second
	 * @param updateFrequency   Fixed update steps per second (must be ≤ targetFps)
	 * @param maxUpdatesPerFrame Maximum catch-up steps per frame before the accumulator is reset
	 */
	void Run(const u32 targetFps, const u32 updateFrequency, const u8 maxUpdatesPerFrame = 5);

	/**
	 * @brief Returns how long the average update loop took in milliseconds
	 */
	double GetUpdateTime() const;

	/**
	 * @brief Returns how long the average draw loop took in milliseconds
	 */
	double GetDrawTime() const;

	/**
	 * @brief Returns the active engine instance
	 *
	 * Asserts if no Engine has been constructed. Prefer the convenience macros
	 * (REGISTRY, RENDERER, etc.) over calling this directly.
	 *
	 * @return Reference to the singleton Engine
	 */
	static Engine& Get();

	/// Entity-component registry
	Registry& registry = m_registry;

	/// Event dispatcher
	Dispatcher& dispatcher = m_dispatcher;

	/// Sprite renderer
	Renderer& renderer = m_renderer;

	/// Resource cache manager
	ResourceManager& resourceManager = m_resourceManager;

	/// Scene lifecycle manager
	SceneManager& sceneManager = m_sceneManager;

	/// System execution manager
	SystemManager& systemManager = m_systemManager;

	/// Lua scripting manager
	LuaManager& luaManager = m_luaManager;

#ifndef __EMSCRIPTEN__
	/// Asynchronous networking (unavailable on Emscripten)
	AsyncNetwork& network = m_network;

	/// Thread pool for background tasks (unavailable on Emscripten)
	BS::thread_pool<BS::tp::none> threadPool;
#endif

private:

	static void SetFlags(const WindowInfo& windowInfo);

	void RaylibResourceManager();

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

#ifndef __EMSCRIPTEN__
	AsyncNetwork m_network;
#endif

	// Timings
	double m_updateTime = 0;
	double m_drawTime = 0;

	// Canvas
	RenderTexture2D m_canvas;
	u32 m_virtualWidth = 0;
	u32 m_virtualHeight = 0;

	bool m_running = true;

	static inline Engine* s_engine = nullptr;
};