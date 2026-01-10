#include "Game.h"

#include "Context.h"
#include "raylib.h"

#include "Renderer.h"
#include "AnimationSystem.h"

#include "Log/Timer.h"

Game::Game(const u32 windowWidth, const u32 windowHeight, const char* windowTitle) :
_renderer(_registry)
{
	SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_HIGHDPI | FLAG_WINDOW_ALWAYS_RUN);

	SetTraceLogLevel(LOG_WARNING);
	
	InitWindow(windowWidth, windowHeight, windowTitle);
	SetExitKey(KEY_NULL);

	_context.emplace(_registry, _dispatcher, _resourceManager, _sceneManager, _systemManager, _luaManager, _networkManager, _logger);
	_sceneManager.SetContext(_context.value());
	_systemManager.SetContext(_context.value());

	// Systems
	_systemManager.AddSystem<AnimationSystem>(0);

	// Set event catcher
	_dispatcher.sink<Event::CloseGame>().connect<&Game::OnCloseGameEvent>(this);
}

Game::~Game()
{
	_resourceManager.ClearCaches();

	_context->registry.clear();

	CloseWindow();
}

void Game::Run(const u32 targetFps, const u32 updateFrequency, const u8 maxUpdatesPerFrame)
{
	Assert(targetFps, "Target fps must be positive");
	Assert(targetFps <= 1000, "Target fps must not be above 1000");

	Assert(updateFrequency, "Update frequency must be positive");
	Assert(updateFrequency <= targetFps, "Update frequency must be at or bellow target fps");

	Assert(maxUpdatesPerFrame, "Must have at least one update per frame");

	SetTargetFPS(targetFps);

	const float timeStep = std::max(1.0f / updateFrequency, 1.0f / targetFps);
	float accummulator = 0.0;

	RollingAverage<double> updateTimeAverage;
	RollingAverage<double> drawTimeAverage;

	while(_running && !WindowShouldClose())
	{
		float deltaT = std::min(GetFrameTime(), 0.1f);
		accummulator += deltaT;

		Timer updateTimer;
		updateTimer.Start();

		u8 steps = 0;
		while (accummulator >= timeStep && steps < maxUpdatesPerFrame)
		{
			_systemManager.Update(timeStep);

			_luaManager.Update(timeStep);

			_sceneManager.Update(timeStep);

			accummulator -= timeStep;
		}

		if (steps >= maxUpdatesPerFrame)
		{
			accummulator = 0;
		}

		_renderer.Update(_registry);

		updateTimeAverage += updateTimer.Stop();
		_context->updateTime = updateTimeAverage.Average();

		Timer drawTimer;
		drawTimer.Start();

		BeginDrawing();
		ClearBackground(BLANK);

		_renderer.Draw(_registry);

		_systemManager.Draw();

		_sceneManager.Draw();

		EndDrawing();

		drawTimeAverage += drawTimer.Stop();
		_context->drawTime = drawTimeAverage.Average();
	}
}

void Game::OnCloseGameEvent(const Event::CloseGame& event)
{
	_running = false;
}