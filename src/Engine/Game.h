#pragma once

#include "Engine/Components.h"
#include "Types.h"

#include "Context.h"

#include "Renderer.h"

#include <optional>
#include <atomic>

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

	void RenderSync();

	// Event handeling
	void OnCloseGameEvent(const Event::CloseGame& event);

private:

	std::optional<Context> _context;

	// Ecs
	Registry _registry;
	Dispathcer _dispatcher;

	// Core systems
	Renderer _renderer;
	ResourceManager _resourceManager;
	SceneManager _sceneManager;
	SystemManager _systemManager;
	LuaManager _luaManager;
	Logger _logger;

	// Rendering
	std::vector<std::pair<Component::Sprite, Component::Transform>> _renderBuffers[2];
	std::atomic<u8> _renderReadIndex = 0;
	std::atomic<i8> _renderWriteIndex = 0;
	std::atomic<bool> _renderComplete = true;
	std::atomic<bool> _updateComplete = false;

	std::atomic<bool> _running = true;
};