#include "Engine.h"

#include "raylib.h"

#include "AnimationSystem.h"
#include "Renderer.h"

#include "Log/Timer.h"

static Engine* engine = nullptr;

Engine::Engine(const WindowInfo& windowInfo) :
m_renderer(m_registry)
{
	engine = this;

	SetFlags(windowInfo);

	SetTraceLogLevel(LOG_WARNING);

	InitWindow(windowInfo.width, windowInfo.height, windowInfo.title.c_str());
	SetExitKey(KEY_NULL);

	// Systems
	m_systemManager.AddSystem<AnimationSystem>(0);

	// Set event catcher
	m_dispatcher.sink<Event::CloseGame>().connect<&Engine::OnCloseGameEvent>(this);
}

Engine::~Engine()
{
	m_resourceManager.ClearCaches();

	m_registry.clear();

	CloseWindow();

	engine = nullptr;
}

void Engine::Run(const u32 targetFps, const u32 updateFrequency, const u8 maxUpdatesPerFrame)
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

	while (m_running && !WindowShouldClose())
	{
		float deltaT = std::min(GetFrameTime(), 0.1f);
		accummulator += deltaT;

		Timer updateTimer;
		updateTimer.Start();

		u8 steps = 0;
		while (accummulator >= timeStep && steps < maxUpdatesPerFrame)
		{
			m_systemManager.Update(timeStep);

			m_luaManager.Update(timeStep);

			m_sceneManager.Update(timeStep);

			accummulator -= timeStep;
		}

		if (steps >= maxUpdatesPerFrame)
		{
			accummulator = 0;
		}

		m_renderer.Update(m_registry);

		updateTimeAverage += updateTimer.Stop();
		m_updateTime = updateTimeAverage.Average();

		Timer drawTimer;
		drawTimer.Start();

		BeginDrawing();
		ClearBackground(BLANK);

		m_renderer.Draw(m_registry);

		m_systemManager.Draw();

		m_sceneManager.Draw();

		EndDrawing();

		drawTimeAverage += drawTimer.Stop();
		m_drawTime = drawTimeAverage.Average();
	}
}

double Engine::GetUpdateTime() const
{
	return m_updateTime;
}

double Engine::GetDrawTime() const
{
	return m_drawTime;
}

Engine& Engine::Get()
{
	Assert(engine);
	return *engine;
}

void Engine::SetFlags(const WindowInfo& windowInfo)
{
	u32 flags = 0;

	if (windowInfo.fullscreen)
	{
		flags |= FLAG_FULLSCREEN_MODE;
	}

	if (windowInfo.borderlessFullscreen)
	{
		flags |= FLAG_BORDERLESS_WINDOWED_MODE;
	}

	if (windowInfo.resizable)
	{
		flags |= FLAG_WINDOW_RESIZABLE;
	}

	if (windowInfo.undecorated)
	{
		flags |= FLAG_WINDOW_UNDECORATED;
	}

	if (windowInfo.topmost)
	{
		flags |= FLAG_WINDOW_TOPMOST;
	}

	if (windowInfo.alwaysRun)
	{
		flags |= FLAG_WINDOW_ALWAYS_RUN;
	}

	if (windowInfo.transparent)
	{
		flags |= FLAG_WINDOW_TRANSPARENT;
	}

	if (windowInfo.highDpi)
	{
		flags |= FLAG_WINDOW_HIGHDPI;
	}

	if (windowInfo.msaa4x)
	{
		flags |= FLAG_MSAA_4X_HINT;
	}

	SetConfigFlags(flags);
}

void Engine::OnCloseGameEvent(const Event::CloseGame& event)
{
	m_running = false;
}