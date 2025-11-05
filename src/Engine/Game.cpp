#include "Engine/Game.h"

#include "Raylib/raylib.h"

#include "Renderer.h"

Game::Game(const u32 windowWidth, const u32 windowHeight, const char* windowTitle)
{
	SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_HIGHDPI | FLAG_WINDOW_ALWAYS_RUN);

	SetTraceLogLevel(LOG_WARNING);
	
	InitWindow(windowWidth, windowHeight, windowTitle);
	SetExitKey(KEY_NULL);
}

Game::~Game()
{
	CloseWindow();
}

void Game::Run(const u32 targetFps)
{
	SetTargetFPS(targetFps);

	while(!WindowShouldClose())
	{
		float deltaT = GetFrameTime();

		_systemManager.Update(deltaT);

		_sceneManager.Update(deltaT);

		BeginDrawing();
		ClearBackground(BLANK);

		_systemManager.Draw();

		_sceneManager.Draw();

		_renderer.Draw(_registry);

		EndDrawing();
	}
}