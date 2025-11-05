#include "Engine/Context.h"
#include "Engine/Game.h"

#include "Raylib/raylib.h"

class FirstScene : public Scene
{
public:

	FirstScene(const Context& context) :
	Scene(context)
	{
		
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
		DrawCircle(150, 150, 25, RED);

		DrawFPS(10, 10);
	}

	void OnEnter() override
	{

	}

	void OnExit() override
	{

	}
};