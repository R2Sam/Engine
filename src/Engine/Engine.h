#pragma once

#include "Types.h"

#include "LuaManager.h"
#include "NetworkManager.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include "SceneManager.h"
#include "SystemManager.h"
#include "entt/entt.h"

using Entity = entt::entity;
using Registry = entt::registry;
using Dispatcher = entt::dispatcher;

namespace Event
{
	struct CloseGame
	{
		static void LuaRegister(sol::state& lua)
		{
			Lua::RegisterType<Event::CloseGame>(lua, DemangleWithoutNamespace<Event::CloseGame>().c_str(),
			sol::constructors<CloseGame()>());
		}
	};
}

class Engine
{
public:

	Engine(const u32 windowWidth, const u32 windowHeight, const char* windowTitle);
	~Engine();

	template <typename T, typename... Args>
	void SetFirstScene(const char* name, Args&&... args)
	{
		_sceneManager.AddScene<T>(name, std::forward<Args>(args)...);
		_sceneManager.ChangeScene(name);
	}

	void Run(const u32 targetFps, const u32 updateFrequency, const u8 maxUpdatesPerFrame = 5);

	double GetUpdateTime() const;
	double GetDrawTime() const;

	static Engine& Get();

	Registry& registry = _registry;
	Dispatcher& dispatcher = _dispatcher;

	// Core systems
	Renderer& renderer = _renderer;
	ResourceManager& resourceManager = _resourceManager;
	SceneManager& sceneManager = _sceneManager;
	SystemManager& systemManager = _systemManager;
	LuaManager& luaManager = _luaManager;
	NetworkManager& networkManager = _networkManager;

private:

	// Event handling
	void OnCloseGameEvent(const Event::CloseGame& event);

	// Ecs
	Registry _registry;
	Dispatcher _dispatcher;

	// Core systems
	Renderer _renderer;
	ResourceManager _resourceManager;
	SceneManager _sceneManager;
	SystemManager _systemManager;
	LuaManager _luaManager;
	NetworkManager _networkManager;

	// Timers
	double _updateTime = 0;
	double _drawTime = 0;

	bool _running = true;
};