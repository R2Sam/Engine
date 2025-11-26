#include "Engine/Game.h"

#include "Engine/Context.h"
#include "Raylib/raylib.h"

#include "Renderer.h"

Game::Game(const u32 windowWidth, const u32 windowHeight, const char* windowTitle)
{
	SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_HIGHDPI | FLAG_WINDOW_ALWAYS_RUN);

	SetTraceLogLevel(LOG_WARNING);
	
	InitWindow(windowWidth, windowHeight, windowTitle);
	SetExitKey(KEY_NULL);

	_context.emplace(_registry, _dispatcher, _renderer, _resourceManager, _sceneManager, _systemManager, _luaManager, _logger);
	_sceneManager.SetContext(_context.value());
	_systemManager.SetContext(_context.value());
	_luaManager.SetContext(_context.value());

	// Set event catcher
	_dispatcher.sink<Event::CloseGame>().connect<&Game::OnCloseGameEvent>(this);
}

Game::~Game()
{
	_resourceManager.ClearCaches();

	_context->registry.clear();

	CloseWindow();
}

void Game::Run(const u32 targetFps)
{
	Assert(targetFps, "Target fps must be positive");
	Assert(targetFps <= 1000, "Target fps must not be above 1000");

	SetTargetFPS(targetFps);

	const float timeStep = 1.0 / targetFps;
	float accummulator = 0.0;

	while(_running && !WindowShouldClose())
	{
		const float deltaT = std::min(GetFrameTime(), 1.0f);
		accummulator += deltaT;

		while (accummulator >= timeStep)
		{
			_systemManager.Update(timeStep);

			_luaManager.Update(timeStep);

			_sceneManager.Update(timeStep);

			accummulator -= timeStep;
		}

		BeginDrawing();
		ClearBackground(BLANK);

		_renderer.Draw(_registry);

		_systemManager.Draw();

		_sceneManager.Draw();

		EndDrawing();
	}
}

void Game::OnCloseGameEvent(const Event::CloseGame& event)
{
	_running = false;
}