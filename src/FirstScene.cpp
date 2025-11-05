#include "Engine/Components.h"
#include "Engine/Context.h"
#include "Engine/Game.h"

#include "Raylib/raylib.h"

class FirstScene : public Scene
{
public:

	FirstScene(const Context& context) :
	Scene(context)
	{
		ball = _context.registry.create();
		_context.registry.emplace<Component::Transform>(ball, Vector2f{100, 100}, Vector2f{250, 250}, 0);
		_context.luaManager.LoadEntityScript(ball, "../src/Script.lua");
	}

	void Update(const float deltaT) override
	{
		if (IsKeyPressed(KEY_ESCAPE))
		{
			_context.dispatcher.trigger<Event::CloseGame>();
		}
	}

	void Draw() override
	{
		Component::Transform& ballTransform = _context.registry.get<Component::Transform>(ball);

		DrawCircle(ballTransform.position.x, ballTransform.position.y, 25, RED);

		DrawFPS(10, 10);
	}

	void OnEnter() override
	{

	}

	void OnExit() override
	{

	}

	entt::entity ball;
};