#include "FirstScene.h"

#include "Engine/Components.h"

#include "Raylib/raylib.h"

FirstScene::FirstScene(const Context& context) :
Scene(context)
{
	ball = _context.registry.create();
	_context.registry.emplace<Component::Transform>(ball, Vector2f{100, 100}, Vector2f{250, 250}, 0);
	_context.luaManager.LoadEntityScript(ball, "../src/Script.lua");

	scriptChangeTime = GetFileModTime("../src/Script.lua");
}

void FirstScene::Update(const float deltaT)
{
	if (IsKeyPressed(KEY_ESCAPE))
	{
		_context.dispatcher.trigger<Event::CloseGame>();
	}

	long currentChangeTime = GetFileModTime("../src/Script.lua");
	if (currentChangeTime > scriptChangeTime)
	{
		scriptChangeTime = currentChangeTime;

		_context.logger.Write(LogLevel::INFO, "Reloading script");
		_context.luaManager.LoadEntityScript(ball, "../src/Script.lua");
	}
}

void FirstScene::Draw()
{
	Component::Transform& ballTransform = _context.registry.get<Component::Transform>(ball);

	DrawCircle(ballTransform.position.x, ballTransform.position.y, 25, RED);

	DrawFPS(10, 10);
}

void FirstScene::OnEnter()
{

}

void FirstScene::OnExit()
{

}