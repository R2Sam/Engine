#pragma once

#include "Types.h"

#include "Context.h"

#include <optional>

class Game
{
public:

	Game(const u32 windowWidth, const u32 windowHeight, const char* windowTitle);
	~Game();

	template<typename T, typename... Args>
	void SetFirstScene(const char *name, Args&&... args)
	{
		_sceneManager.AddScene<T>(name, std::forward<Args>(args)...);
		_sceneManager.ChangeScene(name);
	}

	void Run(const u32 targetFps);

private:

	// Event handeling
	void OnCloseGameEvent(const Event::CloseGame& event);

private:

	std::optional<Context> _context;

	// Ecs
	entt::registry _registry;
	entt::dispatcher _dispatcher;

	// Core systems
	Renderer _renderer;
	ResourceManager _resourceManager;
	SceneManager _sceneManager;
	SystemManager _systemManager;
	LuaManager _luaManager;
	Logger _logger;

	bool _running = true;
};