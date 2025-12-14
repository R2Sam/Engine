#include "FirstScene.h"

#include "Engine/Components.h"

#include "Raylib/raylib.h"

FirstScene::FirstScene(const Context& context) :
Scene(context)
{
	ball = _context.registry.create();
	_context.registry.emplace<Component::Transform>(ball, Vector2f{100, 100}, Vector2f{250, 250}, 0);
	_context.luaManager.LoadScript("../src/Script.lua");

	scriptChangeTime = GetFileModTime("../src/Script.lua");

	_context.luaManager.RegisterEvent<Event::CloseGame>(_context.dispatcher);

	// Basic types
	Lua::RegisterType<Vector2f>(_context.luaManager.lua, "Vector2f",
		sol::constructors<Vector2f(), Vector2f(float, float)>(),
		"x", &Vector2f::x,
		"y", &Vector2f::y);


	// Components
	Lua::RegisterType<Component::Transform>(_context.luaManager.lua, "Transform",
		"position", &Component::Transform::position,
		"velocity", &Component::Transform::velocity,
		"rotation", &Component::Transform::rotation
	);

	// Ecs
	Lua::RegisterType<entt::entity>(_context.luaManager.lua, "Entity",
		"id", entt::to_integral<entt::entity>
	);

	Lua::RegisterFunction(_context.luaManager.lua, "GetTransform", [this](entt::entity entity) -> Component::Transform&
	{
		return _context.registry.get<Component::Transform>(entity);
	});

	Lua::RegisterFunction(_context.luaManager.lua, "HasTransform", [this](entt::entity entity) -> bool
	{
		return _context.registry.all_of<Component::Transform>(entity);
	});
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
		_context.luaManager.LoadScript("../src/Script.lua");
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