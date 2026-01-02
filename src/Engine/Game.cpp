#include "Engine/Game.h"

#include "Engine/Components.h"
#include "Engine/Context.h"
#include "Raylib/raylib.h"

#include "Renderer.h"

#include <chrono>
#include <thread>

Game::Game(const u32 windowWidth, const u32 windowHeight, const char* windowTitle)
{
	SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_HIGHDPI | FLAG_WINDOW_ALWAYS_RUN);

	SetTraceLogLevel(LOG_WARNING);
	
	InitWindow(windowWidth, windowHeight, windowTitle);
	SetExitKey(KEY_NULL);

	_context.emplace(_registry, _dispatcher, _resourceManager, _sceneManager, _systemManager, _luaManager, _logger);
	_sceneManager.SetContext(_context.value());
	_systemManager.SetContext(_context.value());

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

	std::thread updateThread(
	[this, &accummulator, &timeStep]()
	{
		auto last = std::chrono::steady_clock::now();

		while (_running)
		{
			auto now = std::chrono::steady_clock::now();
			const float deltaT = std::chrono::duration<float>(now - last).count();
			accummulator += deltaT;

			while (accummulator >= timeStep)
			{
				_systemManager.Update(timeStep);

				_luaManager.Update(timeStep);

				_sceneManager.Update(timeStep);

				accummulator -= timeStep;
			}

			while(!_renderComplete)
			{
				std::this_thread::yield();
			}

			RenderSync();

			_updateComplete = true;

			_renderComplete = false;
		}
	});

	while(_running && !WindowShouldClose())
	{
		while(!_updateComplete)
		{
			std::this_thread::yield();
		}

		BeginDrawing();
		ClearBackground(BLANK);

		_renderer.Draw(_renderBuffers[_renderReadIndex]);

		_systemManager.Draw();

		_sceneManager.Draw();

		EndDrawing();

		_renderComplete = true;

		_updateComplete = false;
	}

	updateThread.join();
}

void Game::RenderSync() 
{
	std::vector<std::pair<Component::Sprite, Component::Transform>>& buffer = _renderBuffers[_renderWriteIndex];

	buffer.clear();

	auto group = _registry.group<Component::Sprite>(entt::get<const Component::Transform>);

	for (auto [entity, sprite, transform] : group.each())
	{
		buffer.emplace_back(sprite, transform);
	}

	std::sort(buffer.begin(), buffer.end(),
    [](const auto& a, const auto& b) {
        return a.first.layer < b.first.layer;
    });

    u8 oldReadIndex = _renderReadIndex;
	u8 oldWriteIndex = _renderWriteIndex;

	_renderReadIndex = oldWriteIndex;
	_renderWriteIndex = oldReadIndex;
}

void Game::OnCloseGameEvent(const Event::CloseGame& event)
{
	_running = false;
}