#include "Engine.hpp"

#include "raylib.h"
#include "raymath.h"

#include "AnimationSystem.hpp"
#include "Renderer.hpp"

#include "Timing.hpp"

Engine::Engine(const WindowInfo& windowInfo) :
m_renderer(m_registry, windowInfo.virutalWidth, windowInfo.virtualHeight)
{
	Assert(!s_engine, "Only one engine instance may exist at the time");

	s_engine = this;

	SetFlags(windowInfo);

	SetTraceLogLevel(LOG_WARNING);

	InitWindow(windowInfo.width, windowInfo.height, windowInfo.title.c_str());
	SetExitKey(KEY_NULL);

	InitAudioDevice();

	RaylibResourceManager();

	// Systems
	m_systemManager.AddSystem<AnimationSystem>(0);

	// Set event catcher
	m_dispatcher.sink<Event::CloseGame>().connect<&Engine::OnCloseGameEvent>(this);

	m_virtualWidth = windowInfo.virutalWidth;
	m_virtualHeight = windowInfo.virtualHeight;
	m_canvas = LoadRenderTexture(m_virtualWidth, m_virtualHeight);
}

Engine::~Engine()
{
	UnloadRenderTexture(m_canvas);

	m_registry.clear();

	m_systemManager.ClearSystems();
	m_sceneManager.ClearScenes();
	m_resourceManager.ClearCaches();

	CloseAudioDevice();

	CloseWindow();

	s_engine = nullptr;
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

		Stopwatch updateTimer;
		updateTimer.Start();

		// Scaling
		float scaleX;
		float scaleY;
		float scale;
		Vector2 offset;

		static bool s_computedRescale = false;
		if (IsWindowResized() || !s_computedRescale)
		{
			scaleX = GetRenderWidth() / static_cast<float>(m_virtualWidth);
			scaleY = GetRenderHeight() / static_cast<float>(m_virtualHeight);
			scale = std::min(scaleX, scaleY);

			offset = {static_cast<float>((GetRenderWidth() - (m_virtualWidth * scale)) * 0.5),
			static_cast<float>((GetRenderHeight() - (m_virtualHeight * scale)) * 0.5)};

			s_computedRescale = true;
		}

		Vector2 mousePos = GetMousePosition();
		m_virtualMousePos = (mousePos - offset) / scale;

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

		BeginDrawing();
		ClearBackground(BLANK);

		BeginTextureMode(m_canvas);
		{
			ClearBackground(BLANK);

			m_renderer.Draw(m_registry);

			m_systemManager.Draw();

			m_sceneManager.Draw();
		}
		EndTextureMode();

		DrawTexturePro(m_canvas.texture,
		{0, 0, static_cast<float>(m_canvas.texture.width), static_cast<float>(-m_canvas.texture.height)},
		{offset.x, offset.y, m_virtualWidth * scale, m_virtualHeight * scale}, {0, 0}, 0, WHITE);

		EndDrawing();

		drawTimeAverage += GetDrawingTime() * 1000;
		m_drawTime = drawTimeAverage.Average();
	}
}

Vector2 Engine::GetVirtualMousePos() const
{
	return m_virtualMousePos;
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
	Assert(s_engine, "Engine must exists");
	return *s_engine;
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

void Engine::RaylibResourceManager()
{
	m_resourceManager.AddCache<Texture2D>([](const std::string& path) -> std::optional<Texture2D>
	{
		Texture2D texture = LoadTexture(path.c_str());
		if (!IsTextureValid(texture))
		{
			return std::nullopt;
		}

		return texture;
	}, UnloadTexture);

	m_resourceManager.AddCache<Image>([](const std::string& path) -> std::optional<Image>
	{
		Image image = LoadImage(path.c_str());
		if (!IsImageValid(image))
		{
			return std::nullopt;
		}

		return image;
	}, UnloadImage);

	m_resourceManager.AddCache<Wave>([](const std::string& path) -> std::optional<Wave>
	{
		Wave wave = LoadWave(path.c_str());
		if (!IsWaveValid(wave))
		{
			return std::nullopt;
		}

		return wave;
	}, UnloadWave);

	m_resourceManager.AddCache<Sound>([](const std::string& path) -> std::optional<Sound>
	{
		Sound sound = LoadSound(path.c_str());
		if (!IsSoundValid(sound))
		{
			return std::nullopt;
		}

		return sound;
	}, UnloadSound);

	m_resourceManager.AddCache<Music>([](const std::string& path) -> std::optional<Music>
	{
		Music music = LoadMusicStream(path.c_str());
		if (!IsMusicValid(music))
		{
			return std::nullopt;
		}

		return music;
	}, UnloadMusicStream);

	m_resourceManager.AddCache<char*>([](const std::string& path) -> std::optional<char*>
	{
		if (!FileExists(path.c_str()))
		{
			return std::nullopt;
		}

		char* file = LoadFileText(path.c_str());

		if (file)
		{
			return std::nullopt;
		}

		return file;
	}, UnloadFileText);

	m_resourceManager.AddCache<u8*>([](const std::string& path) -> std::optional<u8*>
	{
		if (!FileExists(path.c_str()))
		{
			return std::nullopt;
		}

		i32 size = GetFileLength(path.c_str());
		if (size <= 0)
		{
			return std::nullopt;
		}

		u8* file = LoadFileData(path.c_str(), &size);

		if (file)
		{
			return std::nullopt;
		}

		return file;
	}, UnloadFileData);
}

void Engine::OnCloseGameEvent([[maybe_unused]] const Event::CloseGame& event)
{
	m_running = false;
}