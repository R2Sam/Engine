#pragma once

#include "Components.h"

#include "entt/entt.h"
#include "Raylib/raylib.h"

class Renderer
{
public:

	Renderer();

	void Draw(std::vector<std::pair<Component::Sprite, Component::Transform>>& sprites);

public:

	Camera2D camera;
};