#pragma once

#include "Types.h"

#include "Context.h"

#include <optional>

class Game
{
public:

	Game(const u32 windowWidth, const u32 windowHeight, const char* windowTitle);
	~Game();

	void Run(const u32 targetFps);	// 0 for unlimited

private:

	std::optional<Context> _context;

	// Ecs
	entt::registry _registry;
	entt::dispatcher _dispatcher;

	// Core systems
	Renderer _renderer;
	ResourceManger _resourceManger;
	SceneManager _sceneManager;
	SystemManager _systemManager;
};