#pragma once

#include "Engine/Context.h"

class FirstScene : public Scene
{
public:

	FirstScene(const Context& context);

	void Update(const float deltaT);
	void Draw();

	void OnEnter();
	void OnExit();

	entt::entity ball;

	long scriptChangeTime;
};