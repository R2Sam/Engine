#pragma once

#include "entt/entt.h"
#include "Raylib/raylib.h"

class Renderer
{
public:

	Renderer();

	void Draw(entt::registry& registry);

private:

	void SortSprites(entt::registry& registry);

public:

	Camera2D camera;
};